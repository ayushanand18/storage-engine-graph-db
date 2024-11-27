#pragma once

#ifndef CORE_FLUSHING_MANAGER_H
#define CORE_FLUSHING_MANAGER_H

#include <vector>
#include <memory>
#include "core/memtable.h"

namespace storage_engine {

class FlushingManager {
private:
    std::vector<std::unique_ptr<Memtable>>* old_memtables_;
    bool is_active_;

public:
    FlushingManager();
    FlushingManager(std::vector<std::unique_ptr<Memtable>>* old_memtables);

    // Run the flushing process for old memtables
    void run();

    // Trigger a flush manually
    void triggerFlush();

    // Shutdown the flushing manager (stop further flushing)
    void shutdown();

    // Get the instance of FlushingManager
    FlushingManager* get();
};

} // namespace storage_engine

#endif // CORE_FLUSHING_MANAGER_H
