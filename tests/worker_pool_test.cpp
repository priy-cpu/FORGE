
#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "WorkerPool.h"

int main() {
    // Test 1: Zero workers rejected.
    bool zeroWorkerRejected = false;

    try {
        forge::WorkerPool pool(0);
    }
    catch (const std::invalid_argument&) {
        zeroWorkerRejected = true;
    }

    assert(zeroWorkerRejected);

    // Test 2: Null job rejected.
    {
        forge::WorkerPool pool(1);

        bool nullJobRejected = false;

        try {
            pool.submit(nullptr);
        }
        catch (const std::invalid_argument&) {
            nullJobRejected = true;
        }

        assert(nullJobRejected);

        // Test 3: Shutdown drains pending work.
        auto job = std::make_shared<forge::Job>(
            201, "Shutdown Test", 1
        );

        pool.submit(job);
        pool.shutdown();

        assert(job->getStatus() == forge::JobStatus::COMPLETED);

        // Test 4: Submission after shutdown rejected.
        bool submissionRejected = false;

        try {
            pool.submit(
                std::make_shared<forge::Job>(
                    202, "Rejected Job", 1
                )
            );
        }
        catch (const std::runtime_error&) {
            submissionRejected = true;
        }

        assert(submissionRejected);
    }

    std::cout << "All WorkerPool tests passed!\n";

    return 0;
}
