// memtable.cpp
//
// Implementation of Memtable (in-memory data structure) for the storage engine.

namespace storage_engine {

// TODO: Memtable class definition and implementation

class Memtable {
public:
    std::ostream& serialize();
    void deserialize(std::istream& in);

    // this must be a sync operation
    void insert(std::string new_node_id, GraphNodeMeta& meta_node);

    
};

} // namespace storage_engine
