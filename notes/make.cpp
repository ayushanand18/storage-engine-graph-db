#include <iostream>
using namespace std;

int main() {
    cout << "Path: lib/core/memtable.o lib/core/graph_node.o lib/core/compaction_manager.o lib/core/merge_log.o lib/core/object_cache.o lib/core/sstable.o lib/core/utils.o lib/core/uuid_generator.o lib/index/node_data_index.o lib/index/node_id_index.o lib/persistence/flushing_manager.o lib/persistence/durability_manager.o lib/concurrency/thread_pool.o lib/concurrency/lock_manager.o tests/storage_engine_build_example.o g++ -L../lib   -o  lib/core/memtable.o lib/core/graph_node.o lib/core/compaction_manager.o lib/core/merge_log.o lib/core/object_cache.o lib/core/sstable.o lib/core/utils.o lib/core/uuid_generator.o lib/index/node_data_index.o lib/index/node_id_index.o lib/persistence/flushing_manager.o lib/persistence/durability_manager.o lib/concurrency/thread_pool.o lib/concurrency/lock_manager.o tests/storage_engine_build_example.o" << endl <<
    "/usr/bin/ld: storage_engine_build_example.cpp:(.text+0x1813): undefined reference to `storage_engine::GraphNodeMeta::get_data_id[abi:cxx11]() const" << endl <<
    "/usr/bin/ld: storage_engine_build_example.cpp:(.text+0x1829): undefined reference to `storage_engine::NodeDataIndex::get(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&) const" << endl <<
    "/usr/bin/ld: storage_engine_build_example.cpp:(.text+0x1864): undefined reference to `storage_engine::ObjectCache::put(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, storage_engine::GraphNodeData<void*> const&)'" << endl <<
    "/usr/bin/ld: storage_engine_build_example.cpp:(.text+0x18cc): undefined reference to `storage_engine::CompactionManager::getNodeData(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)'" << endl <<
    "/usr/bin/ld: storage_engine_build_example.cpp:(.text+0x18e5): undefined reference to `storage_engine::GraphNodeData<void*>::GraphNodeData(std::vector<unsigned char, std::allocator<unsigned char> >)'" << endl <<
    "/usr/bin/ld: /tmp/cc2ZvVLt.o: in function `storage_engine::StorageEngine::match_connections(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >)'" << endl;

    cout << "Compiled Successfully" << endl;

}
