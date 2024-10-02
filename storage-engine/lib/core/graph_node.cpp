// node.cpp
//
// Node Class Definitions for the storage engine.

#include "core/uuid_generator.cpp"

namespace storage_engine {

class GraphNodeMeta {
private:
    std::string data_pointer;
    // TODO: replace this set with a RD_Black_Tree Implementation
    //       which is also serializable
    std::set<std::string> connection_list;
public:
    GraphNodeMeta() = default;
    ~GraphNodeMeta() = default;
    void set_data_id(const std::string& data_id) {
        this->data_pointer = data_id;
    }
    void add_connection(const std::string& to_node_id) {
        this->connection_list.insert(to_node_id);
    }
};

template <typename T>
class GraphNodeData {
    std::vector<unsigned char> data;
    std::string node_id;
    std::string data_address_id;
public:
    GraphNodeData() const {
        this->node_id = UUIDGenerator::generateUUID();
    }
    
    GraphNodeData(const std::vector<unsigned char>& data) {
        this->set_data(data);
    }

    GraphNodeData(const T& obj) : data(serialize(obj)) {
        this->node_id = UUIDGenerator::generateUUID();
        // TODO: Init few other data members
    }
    ~GraphNodeData() = default;

    void set_data(const std::string& serialized_data_as_string) const {
        this->data = std::vector<unsigned char>(serialized_data_as_string.begin(), serialized_data_as_string.end());
    }

    void set_data(const std::vector<unsigned char>& serialized_data_as_bytes) noexcept {
        this->data.resize(serialized_data_as_bytes.size());
        std::copy(serialized_data_as_bytes.begin(), serialized_data_as_bytes.end(), this->data.begin());
    }

    std::string get_id() const noexcept {
        return std::static_cast<std::string>(this->node_id);
    }

private:
    std::string data;

    static std::string serialize(const T& obj) {
        std::stringstream ss;

        // Handle different data types
        if constexpr (std::is_same_v<T, int>) {
            ss << obj;
        } else if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << obj << "\"";
        } else if constexpr (std::is_same_v<T, double>) {
            ss << obj;
        } else {
            // Handle other data types or throw an exception
            throw std::runtime_error("Unsupported data type: " + typeid(obj).name());
        }

        return ss.str();
    }
};

} // namespace storage_engine
