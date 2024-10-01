#ifndef STORAGE_ENGINE_H
#define STORAGE_ENGINE_H

#include <string>

namespace storage_engine {

class StorageEngine {
public:
    // Constructor and destructor
    StorageEngine();
    ~StorageEngine();

    // Can be only be moved but not copied
    StorageEngine (StorageEngine&& );
    StorageEngine& operator=(StorageEngine&&); 

    StorageEngine(const MovStorageEngine&) = delete;
    StorageEngine& operator=(const StorageEngine&) = delete;

    std::string create_node(NodeData& );
    std::string add_connection(
        std::string /* from_node_id */, 
        std::string /* to_node_id */
        );
    
    NodeData get_node_data(std::string /* node_id */);
    std::vector<std::string> get_connections_by_class(std::string /* node_class */);

private:
    // in the initial implementation we will have only 1 memtable which
    // shall be extended to multiple threads and multiple memtables each
    // thread will be owner of 1 memtabke. zero contention with the shared
    // northing architecture. reads can be concurrent across multiple threads
    // as they are safe. no writes happen to the segment going to be read
    
    // active memtable, to which all writes should go
    Memtable active_memtable_;

    // list of inactive memtables
    std::vector<Memtable> old_memtables_;
    
    // a merge blog which shall have a buffer, all writes will be flushed to disc
    // from the buffer asynchronously. would be batched to be faster.
    MergeLog merge_log_;

    // SSTables are compacted while in disc to reduce the amount of blocks
    // fetched during a read operation
    CompactionManager compaction_manager_;

    // a csche to speedup reads
    ObjectCache object_cache_;

    // an index from data pointers to raw data
    // this will be flushed to disc directly after a threshold
    // only few number of raw data is kept in-mem
    // data_address_id -> raw_data
    NodeDataIndex node_data_index_;

    // thread pools for CompactionManager, Flushing Manager
    // should spawn 1 thread atleast for each activity
    // writes to active memtable is currently done by 1 thread
    ThreadPool thread_pool_;

    // although zero locks are required in writing to the active memtables
    // compaction and flushing requires acquiring locks so that reads are not phantom
    LockManager lock_manager_;

    // manage flushing to the disc for inactive memtables
    FlushingManager flushing_manager_;

    // handles merge logs and writing it to disc
    DurabilityManager durability_manager_;

    std::vector<std::string> _match_nodeid_with_prefix(std::string /* prefix */);
    std::vector<std::string> _get_connections(std::string /* node_id */);

};

} // namespace storage_engine

#endif // STORAGE_ENGINE_H
