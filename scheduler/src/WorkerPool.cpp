
#include "WorkerPool.h"

#include <iostream>
#include <stdexcept>
#include <syncstream>

namespace forge {

WorkerPool::WorkerPool(
    std::size_t workerCount,
    std::chrono::milliseconds jobDuration,
    bool enableFailures,
    bool enableLogging
)
    : simulatedJobDuration(jobDuration),
      simulateFailures(enableFailures),
      verbose(enableLogging) {

    if (workerCount == 0) {
        throw std::invalid_argument(
            "Worker count must be positive"
        );
    }

    if (jobDuration.count() < 0) {
        throw std::invalid_argument(
            "Job duration cannot be negative"
        );
    }

    workers.reserve(workerCount);

    try {
        for (std::size_t i = 0; i < workerCount; ++i) {
            workers.emplace_back(
                &WorkerPool::workerLoop,
                this,
                i + 1
            );
        }
    }
    catch (...) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            stopping = true;
        }

        condition.notify_all();

        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }

        throw;
    }
}

void WorkerPool::submit(std::shared_ptr<Job> job) {
    if (!job) {
        throw std::invalid_argument(
            "Cannot submit a null job"
        );
    }

    {
        std::lock_guard<std::mutex> lock(mutex);

        if (stopping) {
            throw std::runtime_error(
                "Worker pool is stopping"
            );
        }

        jobQueue.enqueue(job);
        statistics.recordSubmission();
    }

    condition.notify_one();
}

void WorkerPool::workerLoop(std::size_t workerId) {
    while (true) {
        std::shared_ptr<Job> job;

        {
            std::unique_lock<std::mutex> lock(mutex);

            condition.wait(lock, [this] {
                return stopping || !jobQueue.isEmpty();
            });

            if (stopping && jobQueue.isEmpty()) {
                return;
            }

            job = jobQueue.dequeue();
        }

        if (!job) {
            continue;
        }

        constexpr int maxAttempts = 3;

        bool completed = false;

        const auto startTime =
            std::chrono::steady_clock::now();

        for (int attempt = 1; attempt <= maxAttempts; ++attempt) {
            statistics.recordAttempt();

            if (attempt > 1) {
                statistics.recordRetry();
            }

            job->setStatus(JobStatus::RUNNING);

            if (verbose) {
                std::osyncstream(std::cout)
                    << "Worker " << workerId
                    << " executing: " << job->getName()
                    << " | Attempt: " << attempt << '\n';
            }

            try {
                std::this_thread::sleep_for(
                    simulatedJobDuration
                );

                if (simulateFailures &&
                    job->getId() == 103 &&
                    attempt < 3) {

                    throw std::runtime_error(
                        "Temporary payment service error"
                    );
                }

                job->setStatus(JobStatus::COMPLETED);
                completed = true;

                if (verbose) {
                    std::osyncstream(std::cout)
                        << "Job " << job->getId()
                        << " completed successfully!\n";
                }

                break;
            }
            catch (const std::exception& error) {
                if (verbose) {
                    std::osyncstream(std::cout)
                        << "Job " << job->getId()
                        << " failed on attempt " << attempt
                        << ": " << error.what() << '\n';
                }
            }
            catch (...) {
                if (verbose) {
                    std::osyncstream(std::cout)
                        << "Job " << job->getId()
                        << " failed with an unknown exception"
                        << " on attempt " << attempt << '\n';
                }
            }
        }

        const auto endTime =
            std::chrono::steady_clock::now();

        const auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                endTime - startTime
            );

        if (completed) {
            statistics.recordCompletion(duration);
        } else {
            job->setStatus(JobStatus::FAILED);

            statistics.recordFailure(duration);

            if (verbose) {
                std::osyncstream(std::cout)
                    << "Job " << job->getId()
                    << " permanently FAILED after "
                    << maxAttempts << " attempts.\n";
            }
        }
    }
}

void WorkerPool::shutdown() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        stopping = true;
    }

    condition.notify_all();

    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

const JobStatistics& WorkerPool::getStatistics() const {
    return statistics;
}

WorkerPool::~WorkerPool() {
    shutdown();
}

}
