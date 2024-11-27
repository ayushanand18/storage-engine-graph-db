#pragma once

#ifndef CORE_DURABILITY_MANAGER_H
#define CORE_DURABILITY_MANAGER_H

namespace storage_engine {

class DurabilityManager {
public:
    DurabilityManager() = default;
    ~DurabilityManager() = default;

    // Add methods to handle durability operations (e.g., syncing data to disk)
    std::string recoverData(const std::string& filename);
    void persistData(const std::string& filename, const std::string& data);

};

} // namespace storage_engine

#endif // CORE_DURABILITY_MANAGER_H
