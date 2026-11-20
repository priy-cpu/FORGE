
#ifndef FORGE_JOB_QUEUE_H
#define FORGE_JOB_QUEUE_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <queue>
#include <vector>

#include "Job.h"

namespace forge {

class JobQueue {
private:
    struct QueueEntry {
        std::shared_ptr<Job> job;
        std::uint64_t sequence;
    };

    struct ComparePriority {
        bool operator()(const QueueEntry& lhs,
                        const QueueEntry& rhs) const {
            if (lhs.job->getPriority() != rhs.job->getPriority()) {
                return lhs.job->getPriority() < rhs.job->getPriority();
            }

            // Earlier submitted jobs come first for equal priorities.
            return lhs.sequence > rhs.sequence;
        }
    };

    std::priority_queue<
        QueueEntry,
        std::vector<QueueEntry>,
        ComparePriority
    > jobs;

    std::uint64_t nextSequence = 0;

public:
    void enqueue(std::shared_ptr<Job> job);
    std::shared_ptr<Job> dequeue();

    std::shared_ptr<Job> front() const;

    bool isEmpty() const;
    std::size_t size() const;
};

}

#endif
