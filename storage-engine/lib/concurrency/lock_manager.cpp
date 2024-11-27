// lock_manager.cpp
//
// Implementation of LockManager for the storage engine.
// LockManager is not required while writing, but only acquired during
// compaction process to avoid phantom reads

#include "concurrency/lock_manager.h"

namespace storage_engine {

LockManager::LockManager() = default;
LockManager::~LockManager() = default;

std::unique_lock<std::shared_mutex> LockManager::acquireLock(const std::string& node_id) {
    auto& mutex = locks_[node_id];
    return std::unique_lock<std::shared_mutex>(mutex); // Acquire exclusive lock
}

void LockManager::releaseAllLocks() {
    std::lock_guard<std::mutex> guard(global_mutex_); // Ensure thread safety while clearing locks
    locks_.clear(); // Clear all locks
}

} // namespace storage_engine
