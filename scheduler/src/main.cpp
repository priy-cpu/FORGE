
#include <iostream>
#include <memory>
#include <vector>

#include "WorkerPool.h"

const char* getStatusName(forge::JobStatus status) {
    switch (status) {
        case forge::JobStatus::PENDING:
            return "PENDING";

        case forge::JobStatus::RUNNING:
            return "RUNNING";

        case forge::JobStatus::COMPLETED:
            return "COMPLETED";

        case forge::JobStatus::FAILED:
            return "FAILED";
    }

    return "UNKNOWN";
}

int main() {

    std::cout << "====================================\n";
    std::cout << "       FORGE JOB SCHEDULER\n";
    std::cout << "====================================\n";

    // Create jobs with different priorities.
    // Higher numerical value means higher priority.

    auto job1 = std::make_shared<forge::Job>(
        101, "Generate Invoice", 1);

    auto job2 = std::make_shared<forge::Job>(
        102, "Send Email", 2);

    auto job3 = std::make_shared<forge::Job>(
        103, "Process Payment", 3);

    auto job4 = std::make_shared<forge::Job>(
        104, "Generate Report", 1);

    auto job5 = std::make_shared<forge::Job>(
        105, "Backup Database", 2);

    auto job6 = std::make_shared<forge::Job>(
        106, "Send Notification", 3);

    // Retain references to all jobs for final reporting.
    std::vector<std::shared_ptr<forge::Job>> allJobs = {
        job1, job2, job3, job4, job5, job6
    };

    {
        // Create a worker pool with 3 workers.
        forge::WorkerPool pool(3);

        std::cout << "\nStarting 3 workers...\n";
        std::cout << "Submitting 6 jobs...\n\n";

        // Submit jobs.
        pool.submit(job1);
        pool.submit(job2);
        pool.submit(job3);
        pool.submit(job4);
        pool.submit(job5);
        pool.submit(job6);

        // Wait for all jobs to finish.
        pool.shutdown();

        // Display execution statistics.
        pool.getStatistics().printReport();
    }

    // Final job status report.
    std::cout << "\n========== FINAL JOB STATUS ==========\n";

    for (const auto& job : allJobs) {
        std::cout << "Job ID: " << job->getId()
                  << " | Name: " << job->getName()
                  << " | Priority: " << job->getPriority()
                  << " | Status: "
                  << getStatusName(job->getStatus())
                  << '\n';
    }

    std::cout << "\n======================================\n";
    std::cout << "       FORGE EXECUTION FINISHED\n";
    std::cout << "======================================\n";

    return 0;
}
