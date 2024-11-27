// durability_manager.cpp
//
// Implementation of DurabilityManager for the storage engine.

#include "persistence/durability_manager.h"
#include <fstream>
#include <stdexcept>

namespace storage_engine {

DurabilityManager::DurabilityManager() = default;

void DurabilityManager::persistData(const std::string& filename, const std::string& data) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }
    out.write(data.c_str(), data.size());
    out.close();
}

std::string DurabilityManager::recoverData(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Failed to open file for reading: " + filename);
    }
    std::string data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();
    return data;
}

} // namespace storage_engine
