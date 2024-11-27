// object_cache.cpp
//
// Implementation of Object Cache for the storage engine.

#include "core/object_cache.h"
#include <unordered_map>

namespace storage_engine {

ObjectCache::ObjectCache() = default;

void ObjectCache::put(const std::string& key, const GraphNodeData<void*>& data) {
    cache_[key] = data;
}

void ObjectCache::put(const std::string& key, const std::vector<std::string>& connections) {
    // cache_[key] = connections; // Placeholder for future implementation
}

GraphNodeData<void*> ObjectCache::get(const std::string& key, uint8_t& error_code) {
    auto it = cache_.find(key);
    if (it != cache_.end()) {
        error_code = 0; // No error
        return it->second;
    } else {
        error_code = 1; // Cache miss
        return GraphNodeData<void*>(); // Return a default-constructed object
    }
}

void ObjectCache::invalidate(const std::string& key) {
    cache_.erase(key);
}

} // namespace storage_engine
