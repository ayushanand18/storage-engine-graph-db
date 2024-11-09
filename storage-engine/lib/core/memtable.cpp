// memtable.cpp
//
// Implementation of Memtable (in-memory data structure) for the storage engine.

#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include <fstream>
#include <sstream>
#include <atomic>
#include <condition_variable>

namespace storage_engine {

class Memtable {
private:
    // Key-value store for the memtable (node_id -> GraphNodeMeta)
    std::map<std::string, std::shared_ptr<GraphNodeMeta>> table_;
    
    // Mutex for thread-safe operations
    mutable std::mutex mutex_;
    
    // Current size of the memtable in bytes (approximate)
    std::atomic<size_t> size_{0};
    
    // Maximum size before triggering flush (configurable)
    const size_t max_size_;
    
    // Condition variable for signaling when flush is needed
    std::condition_variable flush_needed_;
    
    // Flag to track if memtable is frozen (ready for flush)
    std::atomic<bool> is_frozen_{false};

    // Helper function to calculate entry size
    size_t calculateEntrySize(const std::string& key, const GraphNodeMeta& value) const {
        // Approximate size calculation - can be made more precise if needed
        return key.size() + sizeof(GraphNodeMeta);
    }

public:
    explicit Memtable(size_t max_size = 1024 * 1024) // 1MB default
        : max_size_(max_size) {}
    
    ~Memtable() = default;
    
    // Delete copy constructor and assignment operator
    Memtable(const Memtable&) = delete;
    Memtable& operator=(const Memtable&) = delete;

    // Thread-safe insertion
    void insert(std::string new_node_id, GraphNodeMeta& meta_node) {
        if (is_frozen_) {
            throw std::runtime_error("Cannot insert into frozen memtable");
        }

        std::unique_lock<std::mutex> lock(mutex_);
        
        // Create a shared_ptr to store the meta_node
        auto meta_ptr = std::make_shared<GraphNodeMeta>(meta_node);
        
        // Calculate new entry size
        size_t entry_size = calculateEntrySize(new_node_id, meta_node);
        
        // Check if insertion would exceed max size
        if (size_ + entry_size > max_size_) {
            is_frozen_ = true;
            flush_needed_.notify_one();
            throw std::runtime_error("Memtable full, needs flushing");
        }
        
        // Perform insertion
        table_[std::move(new_node_id)] = std::move(meta_ptr);
        size_ += entry_size;
    }

    // New method to retrieve all entries from the memtable
    std::vector<std::pair<std::string, std::shared_ptr<GraphNodeMeta>>> getEntries() const {
        std::unique_lock<std::mutex> lock(mutex_);
        
        std::vector<std::pair<std::string, std::shared_ptr<GraphNodeMeta>>> entries;
        entries.reserve(table_.size());  // Reserve space to avoid multiple allocations

        for (const auto& [key, value] : table_) {
            entries.emplace_back(key, value);  // Add each entry to the vector
        }

        return entries;  // Return the vector of entries
    }

    // Serialize memtable to output stream
    std::ostream& serialize(std::ostream& out) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // Write number of entries
        size_t count = table_.size();
        out.write(reinterpret_cast<const char*>(&count), sizeof(count));
        
        // Write each entry
        for (const auto& [key, value] : table_) {
            // Write key
            size_t key_size = key.size();
            out.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
            out.write(key.c_str(), key_size);
            
            // Write value (GraphNodeMeta)
            // TODO: Implement proper serialization for GraphNodeMeta
            // This is a placeholder for the actual serialization logic
        }
        
        return out;
    }

    // Deserialize memtable from input stream
    void deserialize(std::istream& in) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // Clear existing data
        table_.clear();
        size_ = 0;
        
        // Read number of entries
        size_t count;
        in.read(reinterpret_cast<char*>(&count), sizeof(count));
        
        // Read each entry
        for (size_t i = 0; i < count; ++i) {
            // Read key
            size_t key_size;
            in.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
            
            std::string key(key_size, '\0');
            in.read(&key[0], key_size);
            
            // Read value (GraphNodeMeta)
            auto meta = std::make_shared<GraphNodeMeta>();
            // TODO: Implement proper deserialization for GraphNodeMeta
            
            // Store in table
            table_[std::move(key)] = std::move(meta);
        }
    }

    // Dump memtable to SSTable
    void dump() {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (table_.empty()) {
            return;
        }
        
        // Create new SSTable
        SSTable ss_table;
        
        // Serialize memtable to SSTable
        std::stringstream ss;
        serialize(ss);
        
        // TODO: Write SSTable to disk
        // This would involve creating a new file with appropriate naming
        // and writing the serialized data to it
        
        // Clear the memtable
        table_.clear();
        size_ = 0;
        is_frozen_ = false;
    }

    bool is_frozen() const noexcept {
        return is_frozen_;
    }
    
    size_t size() const noexcept {
        return size_;
    }
    
    bool empty() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return table_.empty();
    }
    
    size_t count() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return table_.size();
    }

    // Optional: Get value by key (for read operations)
    std::shared_ptr<GraphNodeMeta> get(const std::string& node_id) const {
        std::unique_lock<std::mutex> lock(mutex_);
        auto it = table_.find(node_id);
        if (it != table_.end()) {
            return it->second;
        }
        return nullptr;
    }
};

} // namespace storage_engine