
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <cstdint>

#include "Job.h"
#include "WorkerPool.h"

using namespace forge;

int main() {

    constexpr int PRODUCER_COUNT = 4;
    constexpr int JOBS_PER_PRODUCER = 2500;
    constexpr int TOTAL_JOBS =
        PRODUCER_COUNT * JOBS_PER_PRODUCER;

    std::cout << "\n=== FORGE CONCURRENT STATISTICS TEST ===\n\n";

    WorkerPool pool(
        4,
        std::chrono::milliseconds(0),
        false,
        false
    );

    std::vector<std::vector<std::shared_ptr<Job>>> jobs(
        PRODUCER_COUNT
    );

    std::vector<std::thread> producers;

    for (int p = 0; p < PRODUCER_COUNT; ++p) {

        producers.emplace_back([&, p]() {

            jobs[p].reserve(JOBS_PER_PRODUCER);

            for (int i = 0; i < JOBS_PER_PRODUCER; ++i) {

                const std::uint64_t jobId =
                    static_cast<std::uint64_t>(
                        p * JOBS_PER_PRODUCER + i + 1
                    );

                auto job = std::make_shared<Job>(
                    jobId,
                    "Concurrent Job",
                    1
                );

                pool.submit(job);
                jobs[p].push_back(job);
            }
        });
    }

    for (auto& producer : producers) {
        producer.join();
    }

    std::cout << "All producers finished submitting.\n";

    pool.shutdown();

    int completedJobs = 0;
    int failedJobs = 0;

    for (const auto& producerJobs : jobs) {

        for (const auto& job : producerJobs) {

            if (job->getStatus() == JobStatus::COMPLETED) {
                ++completedJobs;
            } else {
                ++failedJobs;
            }
        }
    }

    const JobStatistics& stats = pool.getStatistics();

    std::cout << "\n========== JOB STATISTICS ==========\n";

    std::cout << "Expected jobs: " << TOTAL_JOBS << '\n';
    std::cout << "Submitted: " << stats.getSubmitted() << '\n';
    std::cout << "Completed: " << stats.getCompleted() << '\n';
    std::cout << "Failed: " << stats.getFailed() << '\n';
    std::cout << "Total Attempts: " << stats.getTotalAttempts() << '\n';
    std::cout << "Retry Attempts: " << stats.getRetriedJobs() << '\n';

    std::cout << "Individual completed jobs: "
              << completedJobs << '\n';

    std::cout << "Individual failed jobs: "
              << failedJobs << '\n';

    std::cout << "====================================\n";

    const bool passed =
        completedJobs == TOTAL_JOBS &&
        failedJobs == 0 &&
        stats.getSubmitted() == TOTAL_JOBS &&
        stats.getCompleted() == TOTAL_JOBS &&
        stats.getFailed() == 0 &&
        stats.getTotalAttempts() == TOTAL_JOBS &&
        stats.getRetriedJobs() == 0;

    if (!passed) {
        std::cerr << "\nFAIL: Statistics verification failed!\n";
        return 1;
    }

    std::cout << "\nPASS: All 10,000 jobs completed!\n";
    std::cout << "PASS: All statistics verified!\n";

    return 0;
}
