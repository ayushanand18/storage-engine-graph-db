#pragma once

#ifndef CORE_NODE_DATA_INDEX_H
#define CORE_NODE_DATA_INDEX_H

#include <unordered_map>
#include <string>
#include <stdexcept>
#include "core/graph_node.h"

namespace storage_engine {

class NodeDataIndex {
public:
    NodeDataIndex() = default;
    ~NodeDataIndex() = default;

    // Insert a new node data entry into the index
    void insert(const std::string& new_node_data_id, const GraphNodeData<void*>& data_node);

    // Retrieve a node data entry from the index
    GraphNodeData<void*> get(const std::string& node_data_id) const;

    // Remove a node data entry from the index
    void remove(const std::string& node_data_id);

private:
    std::unordered_map<std::string, GraphNodeData<void*>> index_; // Map to store node data entries
};

} // namespace storage_engine

#endif // CORE_NODE_DATA_INDEX_H
