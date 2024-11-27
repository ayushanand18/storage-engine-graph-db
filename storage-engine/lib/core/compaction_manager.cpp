// compaction_manager.cpp
//
// Implementation of CompactionManager for the storage engine.

#include "core/compaction_manager.h"
#include <stdexcept>

namespace storage_engine {

CompactionManager::CompactionManager() = default;

CompactionManager::CompactionManager(const std::string& sstable_filename) 
    : sstable_filename_(sstable_filename) {}

void CompactionManager::run() {
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

void CompactionManager::triggerCompaction() {
    run();
}

CompactionManager* CompactionManager::get() {
    return this;
}

std::vector<unsigned char> CompactionManager::getNodeData(const std::string& key) {
    SSTable sstable;
    return sstable.readFromDisk(sstable_filename_, key); // Read from disk using SSTable's method
}

SSTable CompactionManager::mergeOldMemtables() {
    SSTable merged_table;

    for (auto* memtable : old_memtables_) {
        auto entries = memtable->getEntries(); // Assuming getEntries returns a vector of entries

        for (const auto& entry : entries) {
            merged_table.insert(entry); // Insert each entry into the merged table
        }
    }

    return merged_table;
}

void CompactionManager::clearOldMemtables() {
    for (auto* memtable : old_memtables_) {
        delete memtable; // Free memory if dynamically allocated
    }
    old_memtables_.clear(); // Clear the list
}

void CompactionManager::writeSSTableToDisk(const SSTable& table) {
    table.writeToDisk(sstable_filename_); // Use the writeToDisk method from SSTable class
}

} // namespace storage_engine
