
#include <cstdlib>
#include <iostream>
#include <memory>

#include "Job.h"
#include "WorkerPool.h"

using namespace forge;

int main() {

    std::cout << "\n=== FORGE RETRY EXHAUSTION TEST ===\n\n";

    // One worker, zero execution delay,
    // failure simulation enabled, logging disabled.
    WorkerPool pool(
        1,
        std::chrono::milliseconds(0),
        true,
        false
    );

    // Job 206 is divisible by 103.
    // It should fail on all three attempts.
    auto job = std::make_shared<Job>(
        206,
        "Permanent Failure Test",
        1
    );

    pool.submit(job);

    // Wait until all queued work has finished.
    pool.shutdown();

    if (job->getStatus() != JobStatus::FAILED) {
        std::cerr
            << "FAIL: Job was not marked as FAILED\n";

        return 1;
    }

    std::cout
        << "PASS: Permanently failing job marked FAILED\n";

    std::cout
        << "PASS: WorkerPool completed shutdown\n";

    std::cout
        << "\nAll retry exhaustion tests passed!\n";

    return 0;
}
