#pragma once

#ifndef CORE_GRAPH_NODE_H
#define CORE_GRAPH_NODE_H

#include <set>
#include <utility>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include "core/uuid_generator.h"

namespace storage_engine {

class GraphNodeMeta {
private:
    std::string data_pointer;
    std::set<std::pair<std::string, unsigned char>> connection_list;

public:
    GraphNodeMeta() = default;
    ~GraphNodeMeta() = default;

    void set_data_id(const std::string& data_id);
    void add_connection(const std::string& to_node_id, unsigned char flag_byte);
    const std::set<std::pair<std::string, unsigned char>>& get_connections() const;
    std::string get_data_id() const;
};

template <typename T>
class GraphNodeData {
private:
    std::vector<unsigned char> data;
    std::string node_id;
    std::string data_address_id;

public:
    GraphNodeData();
    explicit GraphNodeData(const std::vector<unsigned char> data);
    explicit GraphNodeData(const T& obj);
    ~GraphNodeData() = default;

    void set_data(const std::string& serialized_data_as_string);
    void set_data(const std::vector<unsigned char>& serialized_data_as_bytes) noexcept;
    std::string get_id() const noexcept;

private:
    static std::vector<unsigned char> serialize(const T& obj);
};

} // namespace storage_engine

#endif // CORE_GRAPH_NODE_H
