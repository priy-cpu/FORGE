
#ifndef FORGE_WORKER_POOL_H
#define FORGE_WORKER_POOL_H

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "JobQueue.h"
#include "JobStatistics.h"

namespace forge {

class WorkerPool {
private:
    JobQueue jobQueue;
    JobStatistics statistics;

    std::chrono::milliseconds simulatedJobDuration;
    bool simulateFailures;
    bool verbose;

    std::vector<std::thread> workers;

    std::mutex mutex;
    std::condition_variable condition;

    bool stopping = false;

    // Ensures shutdown and worker joining happen only once.
    std::once_flag shutdownOnce;

    void workerLoop(std::size_t workerId);

public:
    explicit WorkerPool(
        std::size_t workerCount,
        std::chrono::milliseconds jobDuration =
            std::chrono::milliseconds(500),
        bool enableFailures = true,
        bool enableLogging = true
    );

    ~WorkerPool();

    WorkerPool(const WorkerPool&) = delete;
    WorkerPool& operator=(const WorkerPool&) = delete;

    void submit(std::shared_ptr<Job> job);

    void shutdown();

    const JobStatistics& getStatistics() const;
};

}

#endif
