#pragma once

#ifndef CORE_LOCK_MANAGER_H
#define CORE_LOCK_MANAGER_H

#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <string>

namespace storage_engine {

class LockManager {
public:
    LockManager() = default;
    ~LockManager() = default;

    // Acquire a lock for a specific node ID
    std::unique_lock<std::shared_mutex> acquireLock(const std::string& node_id);

    // Release all locks (used during shutdown)
    void releaseAllLocks();

private:
    std::unordered_map<std::string, std::shared_mutex> locks_; // Map of node IDs to their associated locks
    std::mutex global_mutex_; // Mutex to protect access to the lock map
};

} // namespace storage_engine

#endif // CORE_LOCK_MANAGER_H
