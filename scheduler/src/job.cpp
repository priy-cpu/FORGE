
#include "Job.h"

namespace forge {

Job::Job(std::uint64_t jobId,
         const std::string& jobName,
         int jobPriority)
    : id(jobId),
      name(jobName),
      priority(jobPriority),
      status(JobStatus::PENDING) {
}

std::uint64_t Job::getId() const {
    return id;
}

std::string Job::getName() const {
    return name;
}

int Job::getPriority() const {
    return priority;
}

JobStatus Job::getStatus() const {
    return status.load(std::memory_order_acquire);
}

void Job::setStatus(JobStatus newStatus) {
    status.store(newStatus, std::memory_order_release);
}

}
