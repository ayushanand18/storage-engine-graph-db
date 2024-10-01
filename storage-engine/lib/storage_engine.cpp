#include "storage_engine.h"

#include "core/memtable.h"
#include "core/merge_log.h"
#include "core/compaction_manager.h"
#include "core/cache.h"
#include "index/node_data_index.h"
#include "concurrency/thread_pool.h"
#include "concurrency/lock_manager.h"
#include "persistence/flushing_manager.h"
#include "persistence/durability_manager.h"

namespace storage_engine {

StorageEngine::StorageEngine() {
    active_memtable_ = std::make_unique<Memtable>();
    old_memtables = std::make_unique<std::vector<Memtable>>();
    merge_log_ = std::make_unique<MergeLog>();
    compaction_manager_ = std::make_unique<CompactionManager>();
    object_cache_ = std::make_unique<ObjectCache>();
    node_data_index_ = std::make_unique<NodeDataIndex>();
    thread_pool_ = std::make_unique<ThreadPool>();
    lock_manager_ = std::make_unique<LockManager>();
    flushing_manager_ = std::make_unique<FlushingManager>();
    durability_manager_ = std::make_unique<DurabilityManager>();
    
    // start background processes
    thread_pool_->submitTask(std::bind(&CompactionManager::run, compaction_manager_.get()));
    thread_pool_->submitTask(std::bind(&FlushingManager::run, flushing_manager_.get()));
}

StorageEngine::~StorageEngine() {
    // Stop background threads and clean up resources
    // ...
}

StorageEngine::StorageEngine(StorageEngine&& ) noexcept {
    // move other moveable data members
}

StorageEngine::StorageEngine& operator=(StorageEngine&& other) noexcept {
    if (this != &other) {
        // move other moveable data members
    }
    return *this;
}

} // namespace storage_engine
