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
    node_id_index_ = std::make_unique<NodeIDIndex>();
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
    // TODO: Stop background threads and clean up resources
    // ...
}

StorageEngine::StorageEngine(StorageEngine&& ) noexcept {
    // TODO: move other moveable data members
}

StorageEngine& operator=(StorageEngine&& other) noexcept {
    if (this != &other) {
        // TODO: move other moveable data members
    }
    return *this;
}

std::string StorageEngine::create_node(NodeData& node_data) {
    // TODO
}

std::string StorageEngine::add_connection(std::string from_node_id, std::string to_node_id) {
    // TODO
}

void StorageEngine::delete_connection(std::string from_node_id, std::string to_node_id) {
    // TODO
}

void dStorageEngine::elete_node(std::string node_id) {
    // TODO
}

NodeData StorageEngine::get_node_data(std::string node_id) {
    // TODO
}

std::vector<std::string> StorageEngine::match_connections(std::string node_id, std::string condition) {
    if (condition == "") {
        return _get_all_connections;
    } else {
        // sanitize the condition first, only alphanumerics allowed
        this->_sanitize_prefix_for_node_id(condition);

        // pick only that block from the table with given prefix
        std::vector<std::string> connection_list = _get_connections(node_id, condition);
    }
}

void StorageEngine::_sanitize_prefix_for_node_id(std::string& prefix) const {
    // node classes are not more than 20 chars long
    if(prefix.size() > 20) throw std::invalid_argument("prefix strings must be <= 20 chars long");

    // every character in the prefix must be an alphabet
    // numbers or special characters are not allowed
    // this helps us in prefix compression during compaction
    for(char c: prefix) {
        if(not std::isalpha(c)) throw std::invalid_argument("prefix strings must be alphabets");
    }
}


std::vector<std::string> _get_connections(std::string node_id, std::string node_prefix) const {
    // check the cache first for an entry, if a hit, return
    uint8_t cache_error = 0;
    std::vector<std::string> connection_list = _get_connections_from_cache(node_id, node_prefix, cache_error);

    // cache_error means no error
    // ERROR 0 means no error
    // ERROR 1 means cache miss
    // ERROR 2 means stale entry in cache. 
    // remove entry from cache and update
    
    
}




} // namespace storage_engine
