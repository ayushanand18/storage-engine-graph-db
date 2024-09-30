// node.cpp
//
// Node Class Definitions for the storage engine.

namespace storage_engine {

class GraphNodeMeta {
    std::string data_pointer;
    std::set<std::string> connection_list;
public:
    GraphNodeMeta() = default;
    ~GraphNodeMeta() = default;

};

template <typename T>
class GraphNodeData {
    std::vector<unsigned char> data;
    std::string node_id;
    std::string data_address_id;
public:
    GraphNodeData() = default;
    GraphNodeData(const T& obj) : data(serialize(obj)) {
        // TODO: Init few other data members
    }
    ~GraphNodeData() = default;

    void set_data(std::string& serialized_data_as_string) const {
        this->data = std::vector<unsigned char>(serialized_data_as_string.begin(), serialized_data_as_string.end());
    }

    void set_data(std::vector<unsigned char>& serialized_data_as_bytes) noexcept {
        this->data.resize(serialized_data_as_bytes.size());
        std::copy(serialized_data_as_bytes.begin(), serialized_data_as_bytes.end(), this->data.begin());
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
