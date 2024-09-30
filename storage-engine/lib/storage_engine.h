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
    std::vector<std::string> _match_nodeid_with_prefix(std::string /* prefix */);
    std::vector<std::string> _get_connections(std::string /* node_id */);

};

} // namespace storage_engine

#endif // STORAGE_ENGINE_H
