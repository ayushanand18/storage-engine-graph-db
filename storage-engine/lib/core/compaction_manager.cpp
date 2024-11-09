// compaction_manager.cpp
//
// Implementation of CompactionManager for the storage engine.

#include "storage_engine.h" 
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <string>

namespace storage_engine {

class CompactionManager {
public:
    CompactionManager(const std::string& sstable_filename) : sstable_filename_(sstable_filename) {}

    // Run the compaction process
    void run() {
        // Check if there are old memtables to compact
        if (old_memtables_.empty()) {
            return; // Nothing to compact
        }

        // Merge old memtables into a new SSTable
        SSTable new_table = mergeOldMemtables();

        // Clear old memtables after merging
        clearOldMemtables();

        // Write new_table to disk
        writeSSTableToDisk(new_table);
    }

    // Trigger a compaction manually (could be called from outside)
    void triggerCompaction() {
        run();
    }

    CompactionManager* get() {
        return this;
    }

    // Read node data from an SSTable given its key
    std::vector<unsigned char> getNodeData(const std::string& key) {
        SSTable sstable; // Create an instance of SSTable to read from disk
        return sstable.readFromDisk(sstable_filename_, key); // Read from disk using SSTable's method
    }

private:
    std::vector<Memtable*> old_memtables_; // List of old memtables to be compacted
    std::string sstable_filename_; // Filename for the SSTable

    // Merge old memtables into a single SSTable
    SSTable mergeOldMemtables() {
        SSTable merged_table;

        for (auto* memtable : old_memtables_) {
            auto entries = memtable->getEntries(); // Assuming getEntries returns a vector of entries

            for (const auto& entry : entries) {
                merged_table.insert(entry); // Insert each entry into the merged table
            }
        }

        return merged_table;
    }

    // Clear old memtables after compaction
    void clearOldMemtables() {
        for (auto* memtable : old_memtables_) {
            delete memtable; // Free memory if dynamically allocated
        }
        old_memtables_.clear(); // Clear the list
    }

    // Write the newly created SSTable to disk
    void writeSSTableToDisk(const SSTable& table) {
        table.writeToDisk(sstable_filename_); // Use the writeToDisk method from SSTable class
    }
};

} // namespace storage_engine