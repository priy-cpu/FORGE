
#ifndef FORGE_JOB_STATISTICS_H
#define FORGE_JOB_STATISTICS_H

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>

namespace forge {

class JobStatistics {
private:
    std::atomic<std::size_t> submitted{0};
    std::atomic<std::size_t> completed{0};
    std::atomic<std::size_t> failed{0};
    std::atomic<std::size_t> totalAttempts{0};
    std::atomic<std::size_t> retriedJobs{0};

    std::atomic<std::int64_t> totalExecutionTimeMs{0};

public:
    void recordSubmission();

    void recordAttempt();

    void recordRetry();

    void recordCompletion(
        std::chrono::milliseconds duration
    );

    void recordFailure(
        std::chrono::milliseconds duration
    );

    std::size_t getSubmitted() const;
    std::size_t getCompleted() const;
    std::size_t getFailed() const;
    std::size_t getTotalAttempts() const;
    std::size_t getRetriedJobs() const;

    double getTotalExecutionTimeMs() const;
    double getAverageExecutionTimeMs() const;

    void printReport() const;
};

}

#endif

