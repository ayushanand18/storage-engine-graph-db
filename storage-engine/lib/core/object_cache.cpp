// object_cache.cpp
//
// Implementation of Object Cache for the storage engine.

#include <unordered_map>
#include <stdexcept>
#include <vector>

namespace storage_engine {

class ObjectCache {
public:
    ObjectCache() = default;

    // Store an object in the cache
    void put(const std::string& key, const GraphNodeData<void*>& data) {
        cache_[key] = data;
    }

    // Retrieve an object from the cache
    GraphNodeData<void*> get(const std::string& key, uint8_t& error_code) {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            error_code = 0; // No error
            return it->second;
        } else {
            error_code = 1; // Cache miss
            return GraphNodeData<void*>(); // Return a default-constructed object
        }
    }

    // Invalidate an entry in the cache
    void invalidate(const std::string& key) {
        cache_.erase(key);
    }

private:
    std::unordered_map<std::string, GraphNodeData<void*>> cache_; // Map to store cached objects
};

} // namespace storage_engine