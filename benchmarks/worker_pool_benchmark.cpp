
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "WorkerPool.h"

using Clock = std::chrono::steady_clock;

struct BenchmarkResult {
    std::size_t workerCount;
    std::size_t jobCount;
    double executionTimeMs;
    double throughput;
    double speedup;
    double efficiency;
    bool allJobsCompleted;
};

BenchmarkResult runBenchmark(
    std::size_t workerCount,
    std::size_t jobCount,
    double baselineTimeMs
) {
    // 10 ms jobs, no simulated failures, no console logging.
    forge::WorkerPool pool(
        workerCount,
        std::chrono::milliseconds(10),
        false,
        false
    );

    std::vector<std::shared_ptr<forge::Job>> jobs;
    jobs.reserve(jobCount);

    // Prepare jobs before measuring execution.
    for (std::size_t i = 0; i < jobCount; ++i) {
        jobs.push_back(
            std::make_shared<forge::Job>(
                i + 1,
                "Benchmark Job",
                static_cast<int>(i % 3) + 1
            )
        );
    }

    const auto start = Clock::now();

    for (const auto& job : jobs) {
        pool.submit(job);
    }

    pool.shutdown();

    const auto end = Clock::now();

    const double executionTimeMs =
        std::chrono::duration<double, std::milli>(
            end - start
        ).count();

    const double throughput =
        static_cast<double>(jobCount) /
        (executionTimeMs / 1000.0);

    const double speedup =
        baselineTimeMs > 0.0
            ? baselineTimeMs / executionTimeMs
            : 1.0;

    const double efficiency =
        speedup / static_cast<double>(workerCount) * 100.0;

    // Verify every job completed successfully.
    bool allJobsCompleted = true;

    for (const auto& job : jobs) {
        if (job->getStatus() != forge::JobStatus::COMPLETED) {
            allJobsCompleted = false;

            std::cerr
                << "Benchmark job failed: "
                << job->getId() << '\n';
        }
    }

    return {
        workerCount,
        jobCount,
        executionTimeMs,
        throughput,
        speedup,
        efficiency,
        allJobsCompleted
    };
}

int main() {
    constexpr std::size_t jobCount = 100;

    const std::vector<std::size_t> workerCounts = {
        1, 2, 4, 8
    };

    std::vector<BenchmarkResult> results;

    double baselineTimeMs = 0.0;

    std::cout
        << "\n========================================\n"
        << "       FORGE PERFORMANCE BENCHMARK\n"
        << "========================================\n";

    std::cout << "Total Jobs: " << jobCount << '\n';
    std::cout << "Job Duration: 10 ms\n";
    std::cout << "Simulated Failures: Disabled\n";
    std::cout << "Console Logging: Disabled\n";

    for (const auto workerCount : workerCounts) {
        std::cout << "\nRunning with "
                  << workerCount << " workers...\n";

        auto result = runBenchmark(
            workerCount,
            jobCount,
            baselineTimeMs
        );

        if (workerCount == 1) {
            baselineTimeMs = result.executionTimeMs;
            result.speedup = 1.0;
            result.efficiency = 100.0;
        }

        results.push_back(result);

        std::cout << "Completed in "
                  << std::fixed << std::setprecision(2)
                  << result.executionTimeMs
                  << " ms\n";
    }

    std::cout
        << "\n================ RESULTS ================\n";

    std::cout << std::left
              << std::setw(10) << "Workers"
              << std::setw(10) << "Jobs"
              << std::setw(15) << "Time (ms)"
              << std::setw(15) << "Jobs/sec"
              << std::setw(12) << "Speedup"
              << std::setw(12) << "Efficiency"
              << "Verified\n";

    std::cout << std::string(85, '-') << '\n';

    for (const auto& result : results) {
        std::cout << std::fixed << std::setprecision(2);

        std::cout << std::left
                  << std::setw(10) << result.workerCount
                  << std::setw(10) << result.jobCount
                  << std::setw(15) << result.executionTimeMs
                  << std::setw(15) << result.throughput
                  << std::setw(12) << result.speedup
                  << std::setw(12) << result.efficiency
                  << (result.allJobsCompleted ? "PASS" : "FAIL")
                  << '\n';
    }

    std::cout
        << "==========================================\n";

    for (const auto& result : results) {
        if (!result.allJobsCompleted) {
            std::cerr
                << "\nBenchmark failed: Some jobs did not complete.\n";

            return 1;
        }
    }

    std::cout
        << "\nAll benchmark jobs completed successfully!\n";

    return 0;
}
