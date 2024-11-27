// thread_pool.cpp
//
// Implementation of ThreadPool for the storage engine.

#include "concurrency/thread_pool.h"
#include <future>
#include <stdexcept>

namespace storage_engine {

ThreadPool::ThreadPool(size_t numThreads) {
    // Create the specified number of threads
    for (size_t i = 0; i < numThreads; ++i) {
        workers_.emplace_back([this]() {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(queueMutex_);

                    // Wait for a task or shutdown signal
                    condition_.wait(lock, [this]() { return stop_ || !taskQueue_.empty(); });

                    if (stop_ && taskQueue_.empty()) {
                        return;
                    }

                    // Get the next task
                    task = std::move(taskQueue_.front());
                    taskQueue_.pop();
                }

                // Execute the task
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        stop_ = true;
    }

    condition_.notify_all(); // Notify all threads to stop
    for (std::thread& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

std::future<void> ThreadPool::submitTask(std::function<void()> task) {
    auto packaged_task = std::make_shared<std::packaged_task<void()>>(std::move(task));
    std::future<void> future = packaged_task->get_future();

    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        if (stop_) {
            throw std::runtime_error("Cannot submit tasks to a stopped thread pool");
        }
        taskQueue_.emplace([packaged_task]() { (*packaged_task)(); });
    }

    condition_.notify_one(); // Notify one thread to pick up the task
    return future;
}

void ThreadPool::cancelAllTasks() {
    std::unique_lock<std::mutex> lock(queueMutex_);
    stop_ = true;
    taskQueue_ = {}; // Clear the queue
    condition_.notify_all(); // Notify all threads to stop
}

} // namespace storage_engine
