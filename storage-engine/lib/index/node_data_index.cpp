// node_data_index.cpp
//
// Implementation of NodeDataIndex for the storage engine.

#include "index/node_data_index.h"
#include <unordered_map>
#include <stdexcept>

namespace storage_engine {

NodeDataIndex::NodeDataIndex() = default;

void NodeDataIndex::insert(const std::string& new_node_data_id, const GraphNodeData<void*>& data_node) {
    if (data_node.get_id().empty()) {
        throw std::invalid_argument("Data node ID cannot be empty.");
    }
    auto result = index_.emplace(new_node_data_id, data_node);
    if (!result.second) {
        throw std::invalid_argument("Node data ID already exists.");
    }
}

GraphNodeData<void*> NodeDataIndex::get(const std::string& node_data_id) const {
    auto it = index_.find(node_data_id);
    if (it != index_.end()) {
        return it->second;
    } else {
        throw std::invalid_argument("Node data ID does not exist.");
    }
}

void NodeDataIndex::remove(const std::string& node_data_id) {
    auto it = index_.find(node_data_id);
    if (it != index_.end()) {
        index_.erase(it);
    } else {
        throw std::invalid_argument("Node data ID does not exist.");
    }
}

} // namespace storage_engine
