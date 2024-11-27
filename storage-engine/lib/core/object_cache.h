#pragma once

#ifndef CORE_OBJECT_CACHE_H
#define CORE_OBJECT_CACHE_H

#include <unordered_map>
#include <vector>
#include <string>
#include "core/graph_node.h"

namespace storage_engine {

class ObjectCache {
public:
    ObjectCache() = default;
    ~ObjectCache() = default;

    // Store an object in the cache
    void put(const std::string& key, const GraphNodeData<void*>& data);
    void put(const std::string& key, const std::vector<std::string>& connections);

    // Retrieve an object from the cache
    GraphNodeData<void*> get(const std::string& key, uint8_t& error_code);

    // Invalidate an entry in the cache
    void invalidate(const std::string& key);

private:
    std::unordered_map<std::string, GraphNodeData<void*>> cache_; // Map to store cached objects
};

} // namespace storage_engine

#endif // CORE_OBJECT_CACHE_H
