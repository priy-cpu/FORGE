
#ifndef FORGE_JOB_H
#define FORGE_JOB_H

#include <atomic>
#include <cstdint>
#include <string>

namespace forge {

enum class JobStatus {
    PENDING,
    RUNNING,
    COMPLETED,
    FAILED
};

class Job {
private:
    std::uint64_t id;
    std::string name;
    int priority;
    std::atomic<JobStatus> status;

public:
    Job(std::uint64_t jobId, const std::string& jobName, int jobPriority);

    std::uint64_t getId() const;
    std::string getName() const;
    int getPriority() const;
    JobStatus getStatus() const;

    void setStatus(JobStatus newStatus);
};

}

#endif
