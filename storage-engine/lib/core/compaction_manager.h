#pragma once

#ifndef CORE_COMPACTION_MANAGER_H
#define CORE_COMPACTION_MANAGER_H

#include "core/memtable.h"
#include "core/sstable.h"
#include <vector>
#include <string>

namespace storage_engine {

class CompactionManager {
public:
    // Constructors
    CompactionManager();
    explicit CompactionManager(const std::string& sstable_filename);

    // Run the compaction process
    void run();

    // Trigger a manual compaction
    void triggerCompaction();

    // Accessor to return the instance
    CompactionManager* get();

    // Read node data from an SSTable given its key
    std::vector<unsigned char> getNodeData(const std::string& key);

private:
    std::vector<Memtable*> old_memtables_; // List of old memtables to be compacted
    std::string sstable_filename_;        // Filename for the SSTable

    // Merge old memtables into a single SSTable
    SSTable mergeOldMemtables();

    // Clear old memtables after compaction
    void clearOldMemtables();

    // Write the newly created SSTable to disk
    void writeSSTableToDisk(const SSTable& table);
};

} // namespace storage_engine

#endif // CORE_COMPACTION_MANAGER_H
