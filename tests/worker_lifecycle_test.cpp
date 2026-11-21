
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <cstdint>

#include "Job.h"
#include "WorkerPool.h"

using namespace forge;

int main() {

    constexpr int CYCLES = 50;
    constexpr int JOBS_PER_CYCLE = 100;

    int totalCompleted = 0;
    int totalFailed = 0;

    std::cout << "\n=== FORGE WORKER LIFECYCLE TEST ===\n\n";

    // Test repeated worker pool creation and shutdown.
    for (int cycle = 1; cycle <= CYCLES; ++cycle) {

        WorkerPool pool(
            4,
            std::chrono::milliseconds(0),
            false,
            false
        );

        std::vector<std::shared_ptr<Job>> jobs;
        jobs.reserve(JOBS_PER_CYCLE);

        // Submit 100 jobs per lifecycle.
        for (int i = 0; i < JOBS_PER_CYCLE; ++i) {

            const std::uint64_t jobId =
                static_cast<std::uint64_t>(
                    cycle * 1000 + i
                );

            auto job = std::make_shared<Job>(
                jobId,
                "Lifecycle Job",
                1
            );

            pool.submit(job);
            jobs.push_back(job);
        }

        // Two threads request shutdown simultaneously.
        std::thread shutdownThread1([&pool]() {
            pool.shutdown();
        });

        std::thread shutdownThread2([&pool]() {
            pool.shutdown();
        });

        shutdownThread1.join();
        shutdownThread2.join();

        // A third shutdown request must also be harmless.
        pool.shutdown();

        // Verify every job completed.
        for (const auto& job : jobs) {

            if (job->getStatus() == JobStatus::COMPLETED) {
                ++totalCompleted;
            }
            else {
                ++totalFailed;
            }
        }
    }

    std::cout << "Lifecycle cycles: "
              << CYCLES << '\n';

    std::cout << "Total lifecycle jobs: "
              << CYCLES * JOBS_PER_CYCLE << '\n';

    std::cout << "Completed jobs: "
              << totalCompleted << '\n';

    std::cout << "Failed or incomplete jobs: "
              << totalFailed << '\n';

    // Test automatic shutdown through destructor.
    std::vector<std::shared_ptr<Job>> destructorJobs;
    destructorJobs.reserve(100);

    {
        WorkerPool pool(
            4,
            std::chrono::milliseconds(0),
            false,
            false
        );

        for (int i = 0; i < 100; ++i) {

            auto job = std::make_shared<Job>(
                90000 + i,
                "Destructor Test Job",
                1
            );

            pool.submit(job);
            destructorJobs.push_back(job);
        }

        // No explicit shutdown.
        // Destructor must drain queued jobs and join workers.
    }

    int destructorCompleted = 0;
    int destructorFailed = 0;

    for (const auto& job : destructorJobs) {

        if (job->getStatus() == JobStatus::COMPLETED) {
            ++destructorCompleted;
        }
        else {
            ++destructorFailed;
        }
    }

    std::cout << "\n========== DESTRUCTOR TEST ==========\n";

    std::cout << "Expected jobs: 100\n";

    std::cout << "Completed jobs: "
              << destructorCompleted << '\n';

    std::cout << "Failed or incomplete jobs: "
              << destructorFailed << '\n';

    std::cout << "=====================================\n";

    // Final verification.
    const bool passed =
        totalCompleted == CYCLES * JOBS_PER_CYCLE &&
        totalFailed == 0 &&
        destructorCompleted == 100 &&
        destructorFailed == 0;

    if (!passed) {

        std::cerr
            << "\nFAIL: Worker lifecycle test failed!\n";

        return 1;
    }

    std::cout << "\nPASS: Repeated shutdown works!\n";

    std::cout
        << "PASS: Concurrent shutdown requests work!\n";

    std::cout
        << "PASS: Automatic destructor shutdown works!\n";

    std::cout
        << "\nAll lifecycle tests passed!\n";

    return 0;
}
