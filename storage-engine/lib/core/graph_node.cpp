// node.cpp
//
// Node Class Definitions for the storage engine.

#include <set>
#include <utility>
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <type_traits>
#include "storage_engine.h"

namespace storage_engine {

class GraphNodeMeta {
private:
    std::string data_pointer;
    // Replace this set with a RD_Black_Tree Implementation
    // which is also serializable.
    std::set<std::pair<std::string, unsigned char>> connection_list;

public:
    GraphNodeMeta() = default;
    ~GraphNodeMeta() = default;

    void set_data_id(const std::string& data_id) {
        this->data_pointer = data_id;
    }

    void add_connection(const std::string& to_node_id, unsigned char flag_byte) {
        this->connection_list.insert({to_node_id, flag_byte});
    }

    const std::set<std::pair<std::string, unsigned char>>& get_connections() const {
        return connection_list;
    }

    std::string get_data_id() const {
        return data_pointer;
    }
};

template <typename T>
class GraphNodeData {
private:
    std::vector<unsigned char> data; // Serialized data
    std::string node_id; // Unique identifier for the node
    std::string data_address_id; // Address ID for the associated data

public:
    GraphNodeData() : node_id(UUIDGenerator::generateUUID()) {}

    GraphNodeData(const std::vector<unsigned char>& data) : node_id(UUIDGenerator::generateUUID()) {
        this->set_data(data);
    }

    GraphNodeData(const T& obj) : node_id(UUIDGenerator::generateUUID()) {
        this->data = serialize(obj);
        // Initialize other data members if necessary
        this->data_address_id = ""; // Placeholder for future use or implementation
    }

    ~GraphNodeData() = default;

    void set_data(const std::string& serialized_data_as_string) {
        this->data = std::vector<unsigned char>(serialized_data_as_string.begin(), serialized_data_as_string.end());
    }

    void set_data(const std::vector<unsigned char>& serialized_data_as_bytes) noexcept {
        this->data.resize(serialized_data_as_bytes.size());
        std::copy(serialized_data_as_bytes.begin(), serialized_data_as_bytes.end(), this->data.begin());
    }

    std::string get_id() const noexcept {
        return node_id;
    }

private:
    static std::vector<unsigned char> serialize(const T& obj) {
        std::stringstream ss;

        // Handle different data types
        if constexpr (std::is_same_v<T, int>) {
            ss << obj;
        } else if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << obj << "\"";
        } else if constexpr (std::is_same_v<T, double>) {
            ss << obj;
        } else {
            throw std::runtime_error("Unsupported data type: " + typeid(obj).name());
        }

        return std::vector<unsigned char>(ss.str().begin(), ss.str().end());
    }
};

} // namespace storage_engine