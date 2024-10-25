#include "storage_engine.h"

#include "concurrency/thread_pool.cpp"
#include "concurrency/lock_manager.cpp"
#include "core/compaction_manager.cpp"
#include "core/graph_node.cpp"
#include "core/memtable.cpp"
#include "core/merge_log.cpp"
#include "core/object_cache.cpp"
#include "core/sstable.cpp"
#include "core/utils.cpp"
#include "core/uuid_generator.cpp"
#include "index/node_data_index.cpp"
#include "index/node_id_index.cpp"
#include "persistence/flushing_manager.cpp"
#include "persistence/durability_manager.cpp"

#include <memory>
#include <string>
#include <vector>

namespace storage_engine {

StorageEngine::StorageEngine() {
    active_memtable_ = std::make_unique<Memtable>();
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
    thread_pool_->submitTask(std::bind(&CompactionManager::run, compaction_manager_->get()));
    thread_pool_->submitTask(std::bind(&FlushingManager::run, flushing_manager_->get()));
}

StorageEngine::~StorageEngine() {
    // TODO: Stop background threads and clean up resources
}

StorageEngine::StorageEngine(StorageEngine&& ) noexcept {
    // TODO: move other moveable data members
}

StorageEngine& StorageEngine::operator=(StorageEngine&& other) noexcept {
    if (this != &other) {
        // Clean up existing resources (if applicable)

        // Move the other object's resources

        // Optionally, set the moved-from object's members to a safe state
    }
    return *this;
}

std::string StorageEngine::create_node(std::vector<unsigned char>& node_data) {
    GraphNodeData<void*> data_node(node_data);
    
    return _create_node(data_node);
}

std::string StorageEngine::_create_node(const GraphNodeData<void*>& data_node) {
    GraphNodeMeta meta_node;
    std::string new_node_id = UUIDGenerator::generateUUID();

    // first write data to data index
    std::string new_node_data_id = data_node.get_id();
    node_data_index_->insert(new_node_data_id, data_node);

    meta_node.set_data_id(new_node_data_id);

    // then push to active memtable
    active_memtable_->insert(new_node_id, meta_node);

    // add to merge log also
    merge_log_->add(new_node_id, meta_node);
}

void StorageEngine::add_connection(const std::string& from_node_id, const std::string& to_node_id) {
    this->_insert_connection(from_node_id, to_node_id, '1');
}

void StorageEngine::delete_connection(const std::string& from_node_id, const std::string& to_node_id) {
    this->_insert_connection(from_node_id, to_node_id, '0');
}

void StorageEngine::_insert_connection(const std::string& from_node_id, const std::string& to_node_id, unsigned char flag_byte) {
    GraphNodeMeta meta_node;
    // add a connection to the node_id
    meta_node.add_connection(to_node_id, flag_byte);
    // insert this node to active memtable
    active_memtable_->insert(from_node_id, meta_node);

    merge_log_->add(from_node_id, meta_node);
}

void StorageEngine::delete_node(std::string node_id) {
    // TODO
}

GraphNodeData<void*> StorageEngine::get_node_data(const std::string& node_id) {
    // TODO
}

std::vector<std::string> StorageEngine::match_connections(const std::string node_id, std::string condition) {
    if (condition == "") {
        return _get_all_connections(node_id);
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


std::vector<std::string> StorageEngine::_get_connections(const std::string& node_id, const std::string& node_prefix) {
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
