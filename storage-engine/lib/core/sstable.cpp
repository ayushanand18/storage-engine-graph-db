// sstable.cpp
//
// Implementation of SSTable (Sorted String Table) for the storage engine.

#include "core/sstable.h"
#include <fstream>
#include <stdexcept>

namespace storage_engine {

SSTable::SSTable() = default;

void SSTable::insert(const std::pair<std::string, std::shared_ptr<GraphNodeMeta>>& entry) {
    // Serialize GraphNodeMeta into a vector of bytes
    std::vector<unsigned char> serialized_data = serializeGraphNodeMeta(*entry.second);
    // Insert the key-value pair into the map
    table_[entry.first] = serialized_data; // This will maintain sorted order based on keys
}

std::vector<unsigned char> SSTable::serializeGraphNodeMeta(const GraphNodeMeta& meta) {
    // Implement serialization logic for GraphNodeMeta here
    // This is a placeholder for actual serialization logic
    return std::vector<unsigned char>(); // Return empty vector for now
}

void SSTable::writeToDisk(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }
    serialize(out); // Serialize the SSTable contents to the file
    out.close();
}

std::vector<unsigned char> SSTable::readFromDisk(const std::string& filename, const std::string& key) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Failed to open file for reading: " + filename);
    }

    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count)); // Read number of entries

    for (size_t i = 0; i < count; ++i) {
        size_t key_size;
        in.read(reinterpret_cast<char*>(&key_size), sizeof(key_size)); // Read key size

        std::string current_key(key_size, '\0');
        in.read(&current_key[0], key_size); // Read key

        size_t value_size;
        in.read(reinterpret_cast<char*>(&value_size), sizeof(value_size)); // Read value size

        std::vector<unsigned char> value(value_size);
        in.read(reinterpret_cast<char*>(value.data()), value_size); // Read value

        if (current_key == key) {
            return value; // Return the value corresponding to the requested key
        }
    }

    throw std::runtime_error("Key not found in SSTable: " + key);
}

void SSTable::serialize(std::ostream& out) const {
    size_t count = table_.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count)); // Write number of entries

    for (const auto& [key, value] : table_) {
        size_t key_size = key.size();
        out.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size)); // Write key size
        out.write(key.c_str(), key_size); // Write key

        size_t value_size = value.size();
        out.write(reinterpret_cast<const char*>(&value_size), sizeof(value_size)); // Write value size
        out.write(reinterpret_cast<const char*>(value.data()), value_size); // Write value
    }
}

void SSTable::deserialize(std::istream& in) {
    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count)); // Read number of entries

    for (size_t i = 0; i < count; ++i) {
        size_t key_size;
        in.read(reinterpret_cast<char*>(&key_size), sizeof(key_size)); // Read key size

        std::string key(key_size, '\0');
        in.read(&key[0], key_size); // Read key

        size_t value_size;
        in.read(reinterpret_cast<char*>(&value_size), sizeof(value_size)); // Read value size

        std::vector<unsigned char> value(value_size);
        in.read(reinterpret_cast<char*>(value.data()), value_size); // Read value

        table_[key] = std::move(value); // Store the key-value pair
    }
}

std::vector<unsigned char> SSTable::get(const std::string& key) const {
    auto it = table_.find(key);
    if (it != table_.end()) {
        return it->second; // Return the associated value
    }
    throw std::runtime_error("Key not found in SSTable.");
}

} // namespace storage_engine
