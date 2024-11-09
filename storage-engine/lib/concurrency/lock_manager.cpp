// lock_manager.cpp
//
// Implementation of LockManager for the storage engine.
// LockManager is not required while writing, but only acquired during
// compaction process to avoid phantom reads

#include <unordered_map>
#include <mutex>
#include <shared_mutex>

namespace storage_engine {

class LockManager {
public:
    LockManager() = default;

    // Acquire a lock for a specific node ID
    std::unique_lock<std::shared_mutex> acquireLock(const std::string& node_id) {
        auto& mutex = locks_[node_id];
        return std::unique_lock<std::shared_mutex>(mutex); // Acquire exclusive lock
    }

    // Release all locks (used during shutdown)
    void releaseAllLocks() {
        std::lock_guard<std::mutex> guard(global_mutex_); // Ensure thread safety while clearing locks
        locks_.clear(); // Clear all locks
    }

private:
    std::unordered_map<std::string, std::shared_mutex> locks_; // Map of node IDs to their associated locks
    std::mutex global_mutex_; // Mutex to protect access to the lock map
};

} // namespace storage_engine