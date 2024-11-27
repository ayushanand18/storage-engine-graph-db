#pragma once

#ifndef CORE_MERGE_LOG_H
#define CORE_MERGE_LOG_H

#include <string>
#include <iostream>
#include "core/graph_node.h"

namespace storage_engine {

class MergeLog {
public:
    MergeLog() = default;
    ~MergeLog() = default;

    // Serialize the log to an output stream
    std::ostream& serialize();

    // Deserialize the log from an input stream
    void deserialize(std::istream& in);

    // Add a new entry to the log (should maintain the order of arrival)
    void add(const std::string& new_node_id, GraphNodeMeta& meta_node);

    // Write the log to disk
    void toDisc();
};

} // namespace storage_engine

#endif // CORE_MERGE_LOG_H
