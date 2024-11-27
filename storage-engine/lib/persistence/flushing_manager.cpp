// flushing_manager.cpp
//
// Implementation of FlushingManager for the storage engine.

#include "persistence/flushing_manager.h"
#include "core/memtable.h"  // Include necessary headers

namespace storage_engine {

FlushingManager::FlushingManager() = default;

FlushingManager::FlushingManager(std::vector<std::unique_ptr<Memtable>>* old_memtables)
    : old_memtables_(old_memtables), is_active_(true) {}

void FlushingManager::run() {
    if (!is_active_) {
        return;
    }

    // Iterate through old memtables and flush each one
    for (auto& memtable : *old_memtables_) {
        if (memtable) {
            memtable->dump();  // Assuming 'dump' writes the memtable to disk
        }
    }
}

void FlushingManager::triggerFlush() {
    run();
}

void FlushingManager::shutdown() {
    is_active_ = false;  // Disable flushing
}

FlushingManager* FlushingManager::get() {
    return this;
}

} // namespace storage_engine
