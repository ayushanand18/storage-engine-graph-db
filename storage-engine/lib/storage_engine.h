#ifndef STORAGE_ENGINE_H
#define STORAGE_ENGINE_H


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

class StorageEngine {
public:
    // TODO: functions supported (need to be implemented)

    // CREATE a node
    // ADD a connection between two nodes. it is a directed graph
    // DELETE a connection between two nodes.
    // DELETE a node
    // GET data of the node
    // TRAVERSE a node
    // MATCH connection from 1 node, based on some condition
    //       TRAVERSE is same as MATCH if condition is NULL

    // Constructor and destructor
    StorageEngine();
    ~StorageEngine();

    // can be only be moved but not copied
    StorageEngine (StorageEngine&& );
    StorageEngine& operator=(StorageEngine&&); 

    StorageEngine(const StorageEngine&) = delete;
    StorageEngine& operator=(const StorageEngine&) = delete;

    // DEBUG: why is a template T (void *) required?
    std::string create_node(std::vector<unsigned char>& /* node_data */ );
    void add_connection(
        const std::string& /* from_node_id */, 
        const std::string& /* to_node_id */
        );
    void delete_connection(const std::string& /* from_node_id */, const std::string& /* to_node_id */);
    void delete_node(std::string /* node_id */);
    GraphNodeData<void*> get_node_data(const std::string& /* node_id */);

    // throws std::invalid_argument
    std::vector<std::string> match_connections(std::string /* node_id */, std::string /* condition */);
private:
    // in the initial implementation we will have only 1 memtable which
    // shall be extended to multiple threads and multiple memtables each
    // thread will be owner of 1 memtabke. zero contention with the shared
    // northing architecture. reads can be concurrent across multiple threads
    // as they are safe. no writes happen to the segment going to be read
    
    // active memtable, to which all writes should go
    std::unique_ptr<Memtable> active_memtable_;

    // list of inactive memtables
    std::vector<Memtable*> old_memtables_;

    // a merge blog which shall have a buffer, all writes will be flushed to disc
    // from the buffer asynchronously. would be batched to be faster.
    std::unique_ptr<MergeLog> merge_log_;

    // SSTables are compacted while in disc to reduce the amount of blocks
    // fetched during a read operation
    std::unique_ptr<CompactionManager> compaction_manager_;

    // a cache to speedup reads
    std::unique_ptr<ObjectCache> object_cache_;

    // a node id index to contain all nodes that exist
    std::unique_ptr<NodeIDIndex> node_id_index_ ;

    // an index from data pointers to raw data
    // this will be flushed to disc directly after a threshold
    // only few number of raw data is kept in-mem
    // data_address_id -> raw_data
    std::unique_ptr<NodeDataIndex> node_data_index_;

    // thread pools for CompactionManager, Flushing Manager
    // should spawn 1 thread atleast for each activity
    // writes to active memtable is currently done by 1 thread
    std::unique_ptr<ThreadPool> thread_pool_;

    // although zero locks are required in writing to the active memtables
    // compaction and flushing requires acquiring locks so that reads are not phantom
    std::unique_ptr<LockManager> lock_manager_;

    // manage flushing to the disc for inactive memtables
    std::unique_ptr<FlushingManager> flushing_manager_;

    // handles merge logs and writing it to disc
    std::unique_ptr<DurabilityManager> durability_manager_;

    // wether this object is active (ready receiving reads/writes) or not
    bool is_active;

    std::vector<std::string> _get_all_connections(const std::string& /* node_id */);
    std::vector<std::string> _match_nodeid_with_prefix(std::string /* prefix */);
    std::vector<std::string> _get_connections(const std::string& /* node_id */, const std::string& /* prefix_node */);
    std::vector<std::string> _get_connections_from_cache(const std::string& /* node_id */, const std::string& /* prefix_node */, uint8_t /* cache_error */);
    std::vector<std::string> _get_connections_from_active_memtable(std::string /* node_id */, std::string /* node_prefix */);
    std::vector<std::string> _get_connections_from_old_memtables(std::string /* node_id */, std::string /* node_prefix */);
    std::vector<std::string> _get_connections_from_sstables(std::string /* node_id */, std::string /* node_prefix */);
    std::string _create_node(const GraphNodeData<void*>& );

    void _insert_connection(const std::string& /* from_node_id */, const std::string& /* to_node_id */, unsigned char /* flag_byte */);
    void _sanitize_prefix_for_node_id(std::string& /* prefix */) const;
};

} // namespace storage_engine

#endif // STORAGE_ENGINE_H
