// memtable.cpp
//
// Implementation of Memtable (in-memory data structure) for the storage engine.

#include "core/memtable.h"
#include <sstream>
#include <stdexcept>

namespace storage_engine {

Memtable::Memtable(size_t max_size) : max_size_(max_size) {}

Memtable::~Memtable() = default;

void Memtable::insert(std::string new_node_id, GraphNodeMeta& meta_node) {
    if (is_frozen_) {
        throw std::runtime_error("Cannot insert into frozen memtable");
    }

    std::unique_lock<std::mutex> lock(mutex_);
    
    auto meta_ptr = std::make_shared<GraphNodeMeta>(meta_node);
    size_t entry_size = calculateEntrySize(new_node_id, meta_node);
    
    if (size_ + entry_size > max_size_) {
        is_frozen_ = true;
        flush_needed_.notify_one();
        throw std::runtime_error("Memtable full, needs flushing");
    }

    table_[std::move(new_node_id)] = std::move(meta_ptr);
    size_ += entry_size;
}

std::vector<std::pair<std::string, std::shared_ptr<GraphNodeMeta>>> Memtable::getEntries() const {
    std::unique_lock<std::mutex> lock(mutex_);
    
    std::vector<std::pair<std::string, std::shared_ptr<GraphNodeMeta>>> entries;
    entries.reserve(table_.size());

    for (const auto& [key, value] : table_) {
        entries.emplace_back(key, value);
    }

    return entries;
}

std::ostream& Memtable::serialize(std::ostream& out) {
    std::unique_lock<std::mutex> lock(mutex_);
    
    size_t count = table_.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    
    for (const auto& [key, value] : table_) {
        size_t key_size = key.size();
        out.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
        out.write(key.c_str(), key_size);
        
        // Serialize GraphNodeMeta (this is a placeholder for actual serialization)
        // TODO: Implement proper serialization for GraphNodeMeta
    }

    return out;
}

void Memtable::deserialize(std::istream& in) {
    std::unique_lock<std::mutex> lock(mutex_);
    
    table_.clear();
    size_ = 0;
    
    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    
    for (size_t i = 0; i < count; ++i) {
        size_t key_size;
        in.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
        
        std::string key(key_size, '\0');
        in.read(&key[0], key_size);
        
        // Deserialize GraphNodeMeta (this is a placeholder for actual deserialization)
        auto meta = std::make_shared<GraphNodeMeta>();
        // TODO: Implement proper deserialization for GraphNodeMeta
        
        table_[std::move(key)] = std::move(meta);
    }
}

void Memtable::dump() {
    std::unique_lock<std::mutex> lock(mutex_);
    
    if (table_.empty()) {
        return;
    }
    
    // Create and serialize the memtable to SSTable
    SSTable ss_table;
    std::stringstream ss;
    serialize(ss);
    
    // TODO: Write SSTable to disk (Placeholder for actual disk writing logic)
    
    // Clear the memtable
    table_.clear();
    size_ = 0;
    is_frozen_ = false;
}

bool Memtable::is_frozen() const noexcept {
    return is_frozen_;
}

size_t Memtable::size() const noexcept {
    return size_;
}

bool Memtable::empty() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return table_.empty();
}

size_t Memtable::count() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return table_.size();
}

std::shared_ptr<GraphNodeMeta> Memtable::get(const std::string& node_id) const {
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = table_.find(node_id);
    if (it != table_.end()) {
        return it->second;
    }
    return nullptr;
}

size_t Memtable::calculateEntrySize(const std::string& key, const GraphNodeMeta& value) const {
    return key.size() + sizeof(GraphNodeMeta); // Placeholder for actual size calculation
}

} // namespace storage_engine
