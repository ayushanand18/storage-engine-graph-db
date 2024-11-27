// graph_node.cpp
//
// Node Class Definitions for the storage engine.

#include "core/graph_node.h"

namespace storage_engine {

GraphNodeMeta::GraphNodeMeta() = default;
GraphNodeMeta::~GraphNodeMeta() = default;

void GraphNodeMeta::set_data_id(const std::string& data_id) {
    this->data_pointer = data_id;
}

void GraphNodeMeta::add_connection(const std::string& to_node_id, unsigned char flag_byte) {
    this->connection_list.insert({to_node_id, flag_byte});
}

const std::set<std::pair<std::string, unsigned char>>& GraphNodeMeta::get_connections() const {
    return connection_list;
}

std::string GraphNodeMeta::get_data_id() const {
    return data_pointer;
}

template <typename T>
GraphNodeData<T>::GraphNodeData() : node_id(UUIDGenerator::generateUUID()) {}

template <typename T>
GraphNodeData<T>::GraphNodeData(const std::vector<unsigned char> data) : node_id(UUIDGenerator::generateUUID()) {
    this->set_data(data);
}

template <typename T>
GraphNodeData<T>::GraphNodeData(const T& obj) : node_id(UUIDGenerator::generateUUID()) {
    this->data = serialize(obj);
    this->data_address_id = ""; // Placeholder for future use or implementation
}

template <typename T>
void GraphNodeData<T>::set_data(const std::string& serialized_data_as_string) {
    this->data = std::vector<unsigned char>(serialized_data_as_string.begin(), serialized_data_as_string.end());
}

template <typename T>
void GraphNodeData<T>::set_data(const std::vector<unsigned char>& serialized_data_as_bytes) noexcept {
    this->data.resize(serialized_data_as_bytes.size());
    std::copy(serialized_data_as_bytes.begin(), serialized_data_as_bytes.end(), this->data.begin());
}

template <typename T>
std::string GraphNodeData<T>::get_id() const noexcept {
    return node_id;
}

template <typename T>
std::vector<unsigned char> GraphNodeData<T>::serialize(const T& obj) {
    std::stringstream ss;

    if constexpr (std::is_same_v<T, int>) {
        ss << obj;
    } else if constexpr (std::is_same_v<T, std::string>) {
        ss << "\"" << obj << "\"";
    } else if constexpr (std::is_same_v<T, double>) {
        ss << obj;
    } else {
        throw std::runtime_error("Unsupported data type: ");
    }

    return std::vector<unsigned char>(ss.str().begin(), ss.str().end());
}

// Explicit template instantiation
template class GraphNodeData<int>;
template class GraphNodeData<std::string>;
template class GraphNodeData<double>;

} // namespace storage_engine
