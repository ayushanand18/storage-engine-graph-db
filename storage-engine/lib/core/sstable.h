#pragma once

#ifndef CORE_SSTABLE_H
#define CORE_SSTABLE_H

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <fstream>
#include "core/graph_node.h"

namespace storage_engine {

class SSTable {
private:
    std::map<std::string, std::vector<unsigned char>> table_; // Key-value store (node_id -> serialized data)

public:
    SSTable() = default;
    ~SSTable() = default;

    // Insert a new entry into the SSTable
    void insert(const std::pair<std::string, std::shared_ptr<GraphNodeMeta>>& entry);

    // Serialize GraphNodeMeta to bytes (placeholder implementation)
    std::vector<unsigned char> serializeGraphNodeMeta(const GraphNodeMeta& meta);

    // Write the SSTable to disk
    void writeToDisk(const std::string& filename) const;

    // Read an object from the SSTable using its key
    std::vector<unsigned char> readFromDisk(const std::string& filename, const std::string& key);

    // Serialize the SSTable to an output stream
    void serialize(std::ostream& out) const;

    // Deserialize from an input stream (for loading SSTables)
    void deserialize(std::istream& in);

    // Optional: Get a value by key
    std::vector<unsigned char> get(const std::string& key) const;
};

} // namespace storage_engine

#endif // CORE_SSTABLE_H
