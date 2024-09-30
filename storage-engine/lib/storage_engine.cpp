#include "storage_engine.h"

#include "core/lsm_tree.h"
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
    // Initialize components
    lsm_tree_ = std::make_unique<LSMTree>();
    active_memtable_ = std::make_unique<Memtable>();
    merge_log_ = std::make_unique<MergeLog>();
    compaction_manager_ = std::make_unique<CompactionManager>();
    cache_ = std::make_unique<Cache>();
    node_data_index_ = std::make_unique<NodeDataIndex>();
    thread_pool_ = std::make_unique<ThreadPool>();
    lock_manager_ = std::make_unique<LockManager>();
    flushing_manager_ = std::make_unique<FlushingManager>();
    durability_manager_ = std::make_unique<DurabilityManager>();

    // Start background threads
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
