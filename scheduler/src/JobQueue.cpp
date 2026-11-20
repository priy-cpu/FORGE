
#include "JobQueue.h"

#include <stdexcept>
#include <utility>

namespace forge {

void JobQueue::enqueue(std::shared_ptr<Job> job) {
    if (!job) {
        throw std::invalid_argument("Cannot enqueue a null job");
    }

    jobs.push(QueueEntry{
        std::move(job),
        nextSequence++
    });
}

std::shared_ptr<Job> JobQueue::dequeue() {
    if (jobs.empty()) {
        return nullptr;
    }

    auto job = jobs.top().job;
    jobs.pop();

    return job;
}

std::shared_ptr<Job> JobQueue::front() const {
    if (jobs.empty()) {
        return nullptr;
    }

    return jobs.top().job;
}

bool JobQueue::isEmpty() const {
    return jobs.empty();
}

std::size_t JobQueue::size() const {
    return jobs.size();
}

}
