#pragma once

#ifndef CORE_NODE_ID_INDEX_H
#define CORE_NODE_ID_INDEX_H

#include <unordered_set>
#include <string>
#include <stdexcept>

namespace storage_engine {

class NodeIDIndex {
public:
    NodeIDIndex() = default;
    ~NodeIDIndex() = default;

    // Insert a new node ID into the index
    void insert(const std::string& node_id);

    // Remove a node ID from the index
    void remove(const std::string& node_id);

    // Check if a node ID exists in the index
    bool exists(const std::string& node_id) const;

private:
    std::unordered_set<std::string> node_ids_; // Set to store unique node IDs
};

} // namespace storage_engine

#endif // CORE_NODE_ID_INDEX_H
