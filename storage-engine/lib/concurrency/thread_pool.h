#pragma once

#ifndef CORE_THREAD_POOL_H
#define CORE_THREAD_POOL_H

#include <functional>
#include <vector>
#include <thread>
#include <future>
#include <mutex>
#include <queue>

namespace storage_engine {

class ThreadPool {
public:
    ThreadPool(size_t numThreads = std::thread::hardware_concurrency()) {
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

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            stop_ = true;
        }

        condition_.notify_all(); // Notify all threads to stop
        for (std::thread &worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    // Submit a task to the thread pool
    std::future<void> submitTask(std::function<void()> task) {
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

    // Cancel all tasks (mark the pool as stopped)
    void cancelAllTasks() {
        std::unique_lock<std::mutex> lock(queueMutex_);
        stop_ = true;
        taskQueue_ = {}; // Clear the queue
        condition_.notify_all(); // Notify all threads to stop
    }

private:
    std::vector<std::thread> workers_; // Vector of worker threads
    std::queue<std::function<void()>> taskQueue_; // Queue of tasks to be executed
    std::mutex queueMutex_; // Mutex to protect access to the queue
    std::condition_variable condition_; // Condition variable to notify threads
    bool stop_ = false; // Flag to indicate if the pool should stop accepting tasks
};

} // namespace storage_engine

#endif // CORE_THREAD_POOL_H
