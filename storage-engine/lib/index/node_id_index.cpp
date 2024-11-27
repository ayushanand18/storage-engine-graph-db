// node_id_index.cpp
//
// Implementation of NodeIDIndex for the storage engine.

#include "index/node_id_index.h"
#include <unordered_set>
#include <stdexcept>

namespace storage_engine {

NodeIDIndex::NodeIDIndex() = default;

void NodeIDIndex::insert(const std::string& node_id) {
    if (!exists(node_id)) {
        node_ids_.insert(node_id);
    } else {
        throw std::invalid_argument("Node ID already exists.");
    }
}

void NodeIDIndex::remove(const std::string& node_id) {
    auto it = node_ids_.find(node_id);
    if (it != node_ids_.end()) {
        node_ids_.erase(it);
    } else {
        throw std::invalid_argument("Node ID does not exist.");
    }
}

bool NodeIDIndex::exists(const std::string& node_id) const {
    return node_ids_.find(node_id) != node_ids_.end();
}

} // namespace storage_engine
