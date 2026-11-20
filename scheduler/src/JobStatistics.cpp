

#include "JobStatistics.h"

#include <iostream>
#include <iomanip>

namespace forge {

void JobStatistics::recordSubmission() {
    submitted.fetch_add(1);
}

void JobStatistics::recordAttempt() {
    totalAttempts.fetch_add(1);
}

void JobStatistics::recordRetry() {
    retriedJobs.fetch_add(1);
}

void JobStatistics::recordCompletion(
    std::chrono::milliseconds duration
) {
    completed.fetch_add(1);

    totalExecutionTimeMs.fetch_add(
        duration.count()
    );
}

void JobStatistics::recordFailure(
    std::chrono::milliseconds duration
) {
    failed.fetch_add(1);

    totalExecutionTimeMs.fetch_add(
        duration.count()
    );
}

std::size_t JobStatistics::getSubmitted() const {
    return submitted.load();
}

std::size_t JobStatistics::getCompleted() const {
    return completed.load();
}

std::size_t JobStatistics::getFailed() const {
    return failed.load();
}

std::size_t JobStatistics::getTotalAttempts() const {
    return totalAttempts.load();
}

std::size_t JobStatistics::getRetriedJobs() const {
    return retriedJobs.load();
}

double JobStatistics::getTotalExecutionTimeMs() const {
    return static_cast<double>(
        totalExecutionTimeMs.load()
    );
}

double JobStatistics::getAverageExecutionTimeMs() const {
    const auto completedCount = getCompleted();
    const auto failedCount = getFailed();

    const auto finishedCount =
        completedCount + failedCount;

    if (finishedCount == 0) {
        return 0.0;
    }

    return getTotalExecutionTimeMs() /
           static_cast<double>(finishedCount);
}

void JobStatistics::printReport() const {
    std::cout << "\n========== FORGE STATISTICS ==========\n";

    std::cout << "Submitted Jobs: "
              << getSubmitted() << '\n';

    std::cout << "Completed Jobs: "
              << getCompleted() << '\n';

    std::cout << "Failed Jobs: "
              << getFailed() << '\n';

    std::cout << "Total Attempts: "
              << getTotalAttempts() << '\n';

    // Counts retry attempts, not unique retried jobs.
    std::cout << "Retry Attempts: "
              << getRetriedJobs() << '\n';

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Total Execution Time: "
              << getTotalExecutionTimeMs()
              << " ms\n";

    std::cout << "Average Execution Time: "
              << getAverageExecutionTimeMs()
              << " ms\n";

    std::cout << "======================================\n";
}

}
