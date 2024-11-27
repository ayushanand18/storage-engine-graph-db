#pragma once

#ifndef CORE_MEMTABLE_H
#define CORE_MEMTABLE_H

#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include <atomic>
#include <condition_variable>
#include "core/graph_node.h"
#include "core/sstable.h"

namespace storage_engine {

class Memtable {
private:
    std::map<std::string, std::shared_ptr<GraphNodeMeta>> table_;
    mutable std::mutex mutex_;
    std::atomic<size_t> size_{0};
    const size_t max_size_;
    std::condition_variable flush_needed_;
    std::atomic<bool> is_frozen_{false};

    size_t calculateEntrySize(const std::string& key, const GraphNodeMeta& value) const;

public:
    explicit Memtable(size_t max_size = 1024 * 1024);
    ~Memtable();

    // Delete copy semantics
    Memtable(const Memtable&) = delete;
    Memtable& operator=(const Memtable&) = delete;

    // Operations
    void insert(std::string new_node_id, GraphNodeMeta& meta_node);
    void dump();
    bool is_frozen() const noexcept;
    size_t size() const noexcept;
    bool empty() const;
    size_t count() const;
    std::shared_ptr<GraphNodeMeta> get(const std::string& node_id) const;
    std::vector<std::pair<std::string, std::shared_ptr<GraphNodeMeta>>> getEntries() const;

    // Serialization/Deserialization
    std::ostream& serialize(std::ostream& out);
    void deserialize(std::istream& in);
};

} // namespace storage_engine

#endif // CORE_MEMTABLE_H
