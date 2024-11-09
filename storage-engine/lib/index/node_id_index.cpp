// node_id_index.cpp
//
// Implementation of NodeIDIndex for the storage engine.

#include <unordered_set>
#include <stdexcept>

namespace storage_engine {

class NodeIDIndex {
public:
    NodeIDIndex() = default;

    // Insert a new node ID into the index
    void insert(const std::string& node_id) {
        if (!exists(node_id)) {
            node_ids_.insert(node_id);
        } else {
            throw std::invalid_argument("Node ID already exists.");
        }
    }

    // Remove a node ID from the index
    void remove(const std::string& node_id) {
        auto it = node_ids_.find(node_id);
        if (it != node_ids_.end()) {
            node_ids_.erase(it);
        } else {
            throw std::invalid_argument("Node ID does not exist.");
        }
    }

    // Check if a node ID exists in the index
    bool exists(const std::string& node_id) const {
        return node_ids_.find(node_id) != node_ids_.end();
    }

private:
    std::unordered_set<std::string> node_ids_; // Set to store unique node IDs
};

} // namespace storage_engine