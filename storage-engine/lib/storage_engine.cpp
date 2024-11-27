#include "storage_engine.h"

#include <algorithm>
#include <stdexcept>
#include <unordered_set>

namespace storage_engine {

// Keeping existing constructor
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
    
    // set this to an active state
    is_active = true;

    // start background processes
    thread_pool_->submitTask(std::bind(&CompactionManager::run, compaction_manager_.get()));
    thread_pool_->submitTask(std::bind(&FlushingManager::run, flushing_manager_.get()));
}

// Keeping existing destructor
StorageEngine::~StorageEngine() {
    // first set this is no longer active
    is_active = false;

    // then dump all the active memtable first
    active_memtable_->dump();

    // and if there are any tasks in the threadpool, cancel them
    thread_pool_->cancelAllTasks();

    // flush everything to disc first
    // flushing only works on old (inactive) memtables, so it is 
    // essential to first dump all active memtables before flushing
    flushing_manager_->run();

    // push all merge log to disc as well 
    merge_log_->toDisc();

    // Release all locks if held
    lock_manager_->releaseAllLocks();
}

// Keeping existing move operations
StorageEngine::StorageEngine(StorageEngine&& other) 
    : active_memtable_(std::move(other.active_memtable_))
    , old_memtables_(std::move(other.old_memtables_))
    , merge_log_(std::move(other.merge_log_))
    , compaction_manager_(std::move(other.compaction_manager_))
    , object_cache_(std::move(other.object_cache_))
    , node_id_index_(std::move(other.node_id_index_))
    , node_data_index_(std::move(other.node_data_index_))
    , thread_pool_(std::move(other.thread_pool_))
    , lock_manager_(std::move(other.lock_manager_))
    , flushing_manager_(std::move(other.flushing_manager_))
    , durability_manager_(std::move(other.durability_manager_))
    , is_active(other.is_active) {
}

StorageEngine& StorageEngine::operator=(StorageEngine&& other) {
    if (this != &other) {
        active_memtable_ = std::move(other.active_memtable_);
        old_memtables_ = std::move(other.old_memtables_);
        merge_log_ = std::move(other.merge_log_);
        compaction_manager_ = std::move(other.compaction_manager_);
        object_cache_ = std::move(other.object_cache_);
        node_id_index_ = std::move(other.node_id_index_);
        node_data_index_ = std::move(other.node_data_index_);
        thread_pool_ = std::move(other.thread_pool_);
        lock_manager_ = std::move(other.lock_manager_);
        flushing_manager_ = std::move(other.flushing_manager_);
        compaction_manager_ = std::move(other.compaction_manager_);
        is_active = other.is_active;
    }
    return *this;
}

// Keeping existing node creation methods
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
    
    // Add to node ID index
    node_id_index_->insert(new_node_id);

    return new_node_id;
}

// Keeping existing connection methods
void StorageEngine::add_connection(const std::string& from_node_id, const std::string& to_node_id) {
    this->_insert_connection(from_node_id, to_node_id, '1');
}

void StorageEngine::delete_connection(const std::string& from_node_id, const std::string& to_node_id) {
    this->_insert_connection(from_node_id, to_node_id, '0');
}

void StorageEngine::_insert_connection(const std::string& from_node_id, const std::string& to_node_id, unsigned char flag_byte) {
    // Verify both nodes exist
    if (!node_id_index_->exists(from_node_id) || !node_id_index_->exists(to_node_id)) {
        throw std::invalid_argument("One or both nodes don't exist");
    }

    GraphNodeMeta meta_node;
    // add a connection to the node_id
    meta_node.add_connection(to_node_id, flag_byte);
    // insert this node to active memtable
    active_memtable_->insert(from_node_id, meta_node);

    merge_log_->add(from_node_id, meta_node);
    
    // Invalidate cache entries for this connection
    object_cache_->invalidate(from_node_id + "_connections");
}

// Implementing the remaining methods from storage_engine.h
void StorageEngine::delete_node(std::string node_id) {
    if (!node_id_index_->exists(node_id)) {
        throw std::invalid_argument("Node doesn't exist");
    }
    
    auto lock = lock_manager_->acquireLock(node_id);
    
    // Get node data to clean up associated resources
    auto node_data = get_node_data(node_id);
    if (!node_data.get_id().empty()) {
        node_data_index_->remove(node_data.get_id());
    }
    
    // Remove from indexes
    node_id_index_->remove(node_id);
    
    // Invalidate cache
    object_cache_->invalidate(node_id);
    object_cache_->invalidate(node_id + "_connections");
    
    // Mark as deleted in active memtable
    GraphNodeMeta deleted_meta;
    deleted_meta.set_data_id(""); // Empty data ID indicates deletion
    active_memtable_->insert(node_id, deleted_meta);
    
    // Log deletion
    merge_log_->add(node_id, deleted_meta);
}

GraphNodeData<void*> StorageEngine::get_node_data(const std::string& node_id) {
    if (!node_id_index_->exists(node_id)) {
        throw std::invalid_argument("Node doesn't exist");
    }
    
    // Check cache first
    uint8_t cache_error = 0;
    auto cached_data = object_cache_->get(node_id, cache_error);
    if (cache_error == 0) {
        return cached_data;
    }
    
    // Check active memtable
    auto meta = active_memtable_->get(node_id);
    if (meta) {
        auto data = node_data_index_->get(meta->get_data_id());
        object_cache_->put(node_id, data);
        return data;
    }
    
    // Check old memtables
    for (auto* memtable : old_memtables_) {
        meta = memtable->get(node_id);
        if (meta) {
            auto data = node_data_index_->get(meta->get_data_id());
            object_cache_->put(node_id, data);
            return data;
        }
    }
    
    // Check SSTable through compaction manager
    return GraphNodeData<void*>(compaction_manager_->getNodeData(node_id));
}

std::vector<std::string> StorageEngine::match_connections(const std::string node_id, std::string condition) {
    if (!node_id_index_->exists(node_id)) {
        throw std::invalid_argument("Node doesn't exist");
    }

    if (condition.empty()) {
        return _get_all_connections(node_id);
    } else {
        // sanitize the condition first, only alphanumerics allowed
        _sanitize_prefix_for_node_id(condition);
        
        // pick only that block from the table with given prefix
        return _get_connections(node_id, condition);
    }
}

// Implementing the remaining helper methods
std::vector<std::string> StorageEngine::_get_all_connections(const std::string& node_id) {
    std::vector<std::string> all_connections;
    
    // Check cache first
    uint8_t cache_error = 0;
    auto cached_connections = _get_connections_from_cache(node_id, "", cache_error);
    if (cache_error == 0) {
        return cached_connections;
    }
    
    // Combine all sources and remove duplicates
    std::unordered_set<std::string> unique_connections;
    
    // Get from active memtable
    auto active = _get_connections_from_active_memtable(node_id, "");
    unique_connections.insert(active.begin(), active.end());
    
    // Get from old memtables
    auto old = _get_connections_from_old_memtables(node_id, "");
    unique_connections.insert(old.begin(), old.end());
    
    // Get from SSTables
    auto disk = _get_connections_from_sstables(node_id, "");
    unique_connections.insert(disk.begin(), disk.end());
    
    all_connections.assign(unique_connections.begin(), unique_connections.end());
    
    object_cache_->put(node_id + "_connections", all_connections);
    
    return all_connections;
}

std::vector<std::string> StorageEngine::_get_connections(
    const std::string& node_id, const std::string& node_prefix) {
    std::vector<std::string> filtered_connections;
    
    auto all_connections = _get_all_connections(node_id);
    
    // Filter based on prefix
    std::copy_if(all_connections.begin(), all_connections.end(), 
                 std::back_inserter(filtered_connections),
                 [&node_prefix](const std::string& conn) {
                     return conn.find(node_prefix) == 0;
                 });
    
    return filtered_connections;
}

// Implementing the remaining connection retrieval methods
std::vector<std::string> StorageEngine::_get_connections_from_active_memtable(
    std::string node_id, std::string node_prefix) {
    auto meta = active_memtable_->get(node_id);
    if (!meta) return {};
    
    std::vector<std::string> filtered_connections;
    for (const auto& conn : meta->get_connections()) {
        if (node_prefix.empty() || conn.first.find(node_prefix) == 0) {
            if (conn.second != '0') { // Only include non-deleted connections
                filtered_connections.push_back(conn.first);
            }
        }
    }
    return filtered_connections;
}

std::vector<std::string> StorageEngine::_get_connections_from_old_memtables(
    std::string node_id, std::string node_prefix) {
    std::vector<std::string> connections;
    
    for (auto* memtable : old_memtables_) {
        auto meta = memtable->get(node_id);
        if (meta) {
            for (const auto& conn : meta->get_connections()) {
                if (node_prefix.empty() || conn.first.find(node_prefix) == 0) {
                    if (conn.second != '0') { // Only include non-deleted connections
                        connections.push_back(conn.first);
                    }
                }
            }
        }
    }
    
    return connections;
}

std::vector<std::string> StorageEngine::_get_connections_from_sstables(
    std::string node_id, std::string node_prefix) {
    // return compaction_manager_->getConnections(node_id, node_prefix);
    return {};
}

std::vector<std::string> StorageEngine::_get_connections_from_cache(
    const std::string& node_id, const std::string& prefix_node, uint8_t& cache_error) {
    std::string cache_key = node_id + "_connections";
    if (!prefix_node.empty()) {
        cache_key += "_" + prefix_node;
    }
    
    auto cached_data = object_cache_->get(cache_key, cache_error);
    if (cache_error == 0) {
        // return cached_data.get_connections();
    }
    
    return std::vector<std::string>();
}

void StorageEngine::_sanitize_prefix_for_node_id(std::string& prefix) const {
    // node classes are not more than 20 chars long
    if(prefix.size() > 20) {
        throw std::invalid_argument("prefix strings must be <= 20 chars long");
    }

    // every character in the prefix must be an alphabet
    // numbers or special characters are not allowed
    // this helps us in prefix compression during compaction
    for(char c: prefix) {
        if(!std::isalpha(c)) {
            throw std::invalid_argument("prefix strings must be alphabets");
        }
    }
}

// New utility methods
bool StorageEngine::isActive() {
    return is_active;
}

size_t StorageEngine::getActiveMemtableSize() {
    return active_memtable_->size();
}

void StorageEngine::triggerCompaction() {
    compaction_manager_->triggerCompaction();
}

void StorageEngine::triggerFlush() {
    flushing_manager_->triggerFlush();
}

} // namespace storage_engine




// namespace storage_engine {

// StorageEngine::StorageEngine() {
//     active_memtable_ = std::make_unique<Memtable>();
//     merge_log_ = std::make_unique<MergeLog>();
//     compaction_manager_ = std::make_unique<CompactionManager>();
//     object_cache_ = std::make_unique<ObjectCache>();
//     node_id_index_ = std::make_unique<NodeIDIndex>();
//     node_data_index_ = std::make_unique<NodeDataIndex>();
//     thread_pool_ = std::make_unique<ThreadPool>();
//     lock_manager_ = std::make_unique<LockManager>();
//     flushing_manager_ = std::make_unique<FlushingManager>();
//     durability_manager_ = std::make_unique<DurabilityManager>();
    
//     // set this to an active state
//     is_active = true;

//     // start background processes
//     thread_pool_->submitTask(std::bind(&CompactionManager::run, compaction_manager_->get()));
//     thread_pool_->submitTask(std::bind(&FlushingManager::run, flushing_manager_->get()));
// }

// StorageEngine::~StorageEngine() {
//     // first set this is no longer active
//     is_active = true;

//     // then dump all the active memtable first
//     active_memtable_->dump();


//     // and if there are any tasks in the threadpool, cancel them
//     thread_pool_->cancelAllTasks();

//     // flush everything to disc first
//     // flushing only works on old (inactive) memtables, so it is 
//     // essential to first dump all active memtables before flushing
//     flushing_manager_->run();

//     // push all merge log to disc as well 
//     merge_log_->toDisc();

//     // TODO: then release all locks if held
//     lock_manager_->releaseAllLocks();
// }

// StorageEngine::StorageEngine(StorageEngine&& ) noexcept {
//     // TODO: move other moveable data members
// }

// StorageEngine& StorageEngine::operator=(StorageEngine&& other) noexcept {
//     if (this != &other) {
//         // Do nothing, as this operation is SAFE
//     }
//     return *this;
// }

// std::string StorageEngine::create_node(std::vector<unsigned char>& node_data) {
//     GraphNodeData<void*> data_node(node_data);
    
//     return _create_node(data_node);
// }

// std::string StorageEngine::_create_node(const GraphNodeData<void*>& data_node) {
//     GraphNodeMeta meta_node;
//     std::string new_node_id = UUIDGenerator::generateUUID();

//     // first write data to data index
//     std::string new_node_data_id = data_node.get_id();
//     node_data_index_->insert(new_node_data_id, data_node);

//     meta_node.set_data_id(new_node_data_id);

//     // then push to active memtable
//     active_memtable_->insert(new_node_id, meta_node);

//     // add to merge log also
//     merge_log_->add(new_node_id, meta_node);
// }

// void StorageEngine::add_connection(const std::string& from_node_id, const std::string& to_node_id) {
//     this->_insert_connection(from_node_id, to_node_id, '1');
// }

// void StorageEngine::delete_connection(const std::string& from_node_id, const std::string& to_node_id) {
//     this->_insert_connection(from_node_id, to_node_id, '0');
// }

// void StorageEngine::_insert_connection(const std::string& from_node_id, const std::string& to_node_id, unsigned char flag_byte) {
//     GraphNodeMeta meta_node;
//     // add a connection to the node_id
//     meta_node.add_connection(to_node_id, flag_byte);
//     // insert this node to active memtable
//     active_memtable_->insert(from_node_id, meta_node);

//     merge_log_->add(from_node_id, meta_node);
// }

// void StorageEngine::delete_node(std::string node_id) {
//     // TODO
// }

// GraphNodeData<void*> StorageEngine::get_node_data(const std::string& node_id) {
//     // TODO
// }

// std::vector<std::string> StorageEngine::match_connections(const std::string node_id, std::string condition) {
//     if (condition == "") {
//         return _get_all_connections(node_id);
//     } else {
//         // sanitize the condition first, only alphanumerics allowed
//         this->_sanitize_prefix_for_node_id(condition);

//         // pick only that block from the table with given prefix
//         std::vector<std::string> connection_list = _get_connections(node_id, condition);
//     }
// }

// void StorageEngine::_sanitize_prefix_for_node_id(std::string& prefix) const {
//     // node classes are not more than 20 chars long
//     if(prefix.size() > 20) throw std::invalid_argument("prefix strings must be <= 20 chars long");

//     // every character in the prefix must be an alphabet
//     // numbers or special characters are not allowed
//     // this helps us in prefix compression during compaction
//     for(char c: prefix) {
//         if(not std::isalpha(c)) throw std::invalid_argument("prefix strings must be alphabets");
//     }
// }


// std::vector<std::string> StorageEngine::_get_connections(const std::string& node_id, const std::string& node_prefix) {
//     // check the cache first for an entry, if a hit, return
//     uint8_t cache_error = 0;
//     std::vector<std::string> connection_list = _get_connections_from_cache(node_id, node_prefix, cache_error);

//     // cache_error means no error
//     // ERROR 0 means no error
//     // ERROR 1 means cache miss
//     // ERROR 2 means stale entry in cache. 
//     // remove entry from cache and update
        
// }


// } // namespace storage_engine
