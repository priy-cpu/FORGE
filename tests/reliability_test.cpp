
#include <iostream>
#include <memory>
#include <stdexcept>

#include "WorkerPool.h"
#include "Job.h"

using namespace forge;

void testZeroWorkers() {
    try {
        WorkerPool pool(0);

        std::cerr << "FAIL: Zero workers were accepted\n";
        std::exit(1);
    }
    catch (const std::exception&) {
        std::cout << "PASS: Zero workers rejected\n";
    }
}

void testNullJob() {
    WorkerPool pool(1);

    try {
        pool.submit(nullptr);

        std::cerr << "FAIL: Null job was accepted\n";
        std::exit(1);
    }
    catch (const std::exception&) {
        std::cout << "PASS: Null job rejected\n";
    }
}

void testSubmissionAfterShutdown() {
    WorkerPool pool(1);

    pool.shutdown();

    try {
        auto job = std::make_shared<Job>(
            301, "Post Shutdown Test", 1
        );

        pool.submit(job);

        std::cerr << "FAIL: Job accepted after shutdown\n";
        std::exit(1);
    }
    catch (const std::exception&) {
        std::cout << "PASS: Submission after shutdown rejected\n";
    }
}

int main() {
    std::cout << "\n=== FORGE RELIABILITY TESTS ===\n\n";

    testZeroWorkers();
    testNullJob();
    testSubmissionAfterShutdown();

    std::cout << "\nAll reliability tests passed!\n";

    return 0;
}
