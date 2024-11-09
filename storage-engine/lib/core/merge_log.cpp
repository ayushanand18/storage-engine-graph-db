// merge_log.cpp
//
// Implementation of MergeLog (sequential log of writes) for the storage engine.

namespace storage_engine {

// TODO: MergeLog class definition and implementation
class MergeLog {
public:
    std::ostream& serialize();
    void deserialize(std::istream& in);

    // needs to be a async operation but gauranteed to be in order of arrival
    void add(std::string new_node_id, GraphNodeMeta& meta_node);

    void toDisc();
};

} // namespace storage_engine
