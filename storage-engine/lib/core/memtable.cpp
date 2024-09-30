// memtable.cpp
//
// Implementation of Memtable (in-memory data structure) for the storage engine.

namespace storage_engine {

class Memtable {
public:
    std::ostream& serialize();
    void deserialize(std::istream& in);
    
}

// TODO: Memtable class definition and implementation

} // namespace storage_engine
