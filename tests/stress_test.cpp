
#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "Job.h"
#include "WorkerPool.h"

using namespace forge;

int main() {

    constexpr int PRODUCERS = 8;
    constexpr int WORKERS = 8;
    constexpr int JOBS_PER_PRODUCER = 12500;

    constexpr int TOTAL_JOBS =
        PRODUCERS * JOBS_PER_PRODUCER;

    std::cout << "\n====================================\n";
    std::cout << "       FORGE STRESS TEST\n";
    std::cout << "====================================\n";

    std::cout << "Producer threads: " << PRODUCERS << '\n';
    std::cout << "Worker threads: " << WORKERS << '\n';
    std::cout << "Total jobs: " << TOTAL_JOBS << '\n';

    WorkerPool pool(
        WORKERS,
        std::chrono::milliseconds(0),
        false,
        false
    );

    std::vector<std::vector<std::shared_ptr<Job>>> jobs(
        PRODUCERS
    );

    std::vector<std::thread> producers;

    for (int p = 0; p < PRODUCERS; ++p) {

        producers.emplace_back([&, p]() {

            jobs[p].reserve(JOBS_PER_PRODUCER);

            for (int i = 0; i < JOBS_PER_PRODUCER; ++i) {

                const std::uint64_t jobId =
                    static_cast<std::uint64_t>(
                        p * JOBS_PER_PRODUCER + i + 1
                    );

                auto job = std::make_shared<Job>(
                    jobId,
                    "Stress Test Job",
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

    std::cout << "\nAll producers finished.\n";

    pool.shutdown();

    int completed = 0;
    int failed = 0;

    for (const auto& producerJobs : jobs) {

        for (const auto& job : producerJobs) {

            if (job->getStatus() == JobStatus::COMPLETED) {
                ++completed;
            } else {
                ++failed;
            }
        }
    }

    const JobStatistics& stats = pool.getStatistics();

    std::cout << "\n========== STRESS RESULTS ==========\n";

    std::cout << "Expected jobs: " << TOTAL_JOBS << '\n';
    std::cout << "Submitted: " << stats.getSubmitted() << '\n';
    std::cout << "Completed: " << stats.getCompleted() << '\n';
    std::cout << "Failed: " << stats.getFailed() << '\n';
    std::cout << "Total attempts: "
              << stats.getTotalAttempts() << '\n';

    std::cout << "Retry attempts: "
              << stats.getRetriedJobs() << '\n';

    std::cout << "Individual completed: "
              << completed << '\n';

    std::cout << "Individual failed: "
              << failed << '\n';

    std::cout << "====================================\n";

    const bool passed =
        completed == TOTAL_JOBS &&
        failed == 0 &&
        stats.getSubmitted() == TOTAL_JOBS &&
        stats.getCompleted() == TOTAL_JOBS &&
        stats.getFailed() == 0 &&
        stats.getTotalAttempts() == TOTAL_JOBS &&
        stats.getRetriedJobs() == 0;

    if (!passed) {

        std::cerr << "\nFAIL: Stress test failed!\n";
        return 1;
    }

    std::cout << "\nPASS: All 100,000 jobs completed!\n";
    std::cout << "PASS: Statistics verified!\n";
    std::cout << "PASS: No jobs lost or left incomplete!\n";

    return 0;
}
