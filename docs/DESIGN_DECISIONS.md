# FORGE — Design Decisions

## 1. Overview

This document records the major engineering decisions made during the development of FORGE, a C++20 priority-based job scheduling engine.

Each decision includes the chosen approach, its reasoning, and relevant trade-offs.

## 2. C++20

**Decision:** Use C++20 as the implementation standard.

**Reasoning:**
- Provides modern concurrency and synchronization facilities.
- Supports `std::jthread` as a possible future improvement.
- Provides `std::osyncstream` for synchronized console output.
- Offers modern language features and standard library improvements.

**Trade-off:** Requires a compiler with C++20 support.

## 3. Priority Queue Instead of FIFO Queue

**Decision:** Use `std::priority_queue` instead of `std::queue`.

**Reasoning:**

A conventional FIFO queue processes jobs in submission order. FORGE requires higher-priority jobs to be selected before lower-priority jobs.

The priority queue provides efficient access to the highest-priority entry.

**Scheduling complexity:**

| Operation | Complexity |
|---|---:|
| Insert job | O(log N) |
| Retrieve highest-priority job | O(1) |
| Remove highest-priority job | O(log N) |

**Trade-off:** Jobs are not necessarily executed in submission order. Lower-priority jobs may wait longer when higher-priority jobs keep arriving.

## 4. FIFO Ordering for Equal Priorities

**Decision:** Assign a monotonically increasing sequence number to every queue entry.

**Reasoning:**

A priority queue does not automatically preserve insertion order when multiple entries have equal priority.

FORGE uses a sequence number to break priority ties.

Example:

| Job | Priority | Sequence |
|---|---:|---:|
| Job A | 3 | 1 |
| Job B | 3 | 2 |
| Job C | 2 | 3 |

Job A is selected before Job B because both have priority 3, but A was submitted first.

**Trade-off:** Each queue entry requires an additional sequence number.

## 5. std::shared_ptr for Job Ownership

**Decision:** Store jobs using `std::shared_ptr<Job>`.

**Reasoning:**
- The application and worker pool can share ownership of a job.
- Jobs remain alive while workers are processing them.
- Ownership management is handled automatically.
- Avoids copying job objects containing atomic members.

**Trade-off:** Shared ownership introduces reference-counting overhead compared with exclusive ownership.

## 6. std::atomic for Job Status

**Decision:** Use `std::atomic<JobStatus>`.

**Reasoning:**

Multiple threads may read or update job status during execution.

Atomic operations prevent data races on the status variable without requiring a separate mutex for every status access.

The implementation uses acquire semantics for reads and release semantics for writes.

**Trade-off:** Atomic operations provide synchronization for the status field only. They do not automatically make every member of the Job class thread-safe.

## 7. Mutex for Shared Queue Access

**Decision:** Protect JobQueue and the stopping flag using `std::mutex`.

**Reasoning:**

Multiple worker threads access the same queue.

Without synchronization, simultaneous queue modifications could cause data races and undefined behavior.

The WorkerPool mutex protects:
- Job submission.
- Job retrieval.
- Queue state checks.
- Shutdown state.

Workers release the mutex before executing jobs, avoiding unnecessary lock contention during task execution.

**Trade-off:** Only one thread can modify or retrieve queue entries at a time.

## 8. Condition Variable Instead of Busy Waiting

**Decision:** Use `std::condition_variable`.

**Reasoning:**

Workers should sleep when there are no available jobs rather than continuously checking the queue.

The condition variable allows workers to wait until:
- A new job becomes available.
- The pool begins shutting down.

**Trade-off:** Workers need correct notification and shutdown handling to avoid remaining blocked indefinitely.

## 9. Fixed Worker Pool

**Decision:** Create a configurable number of worker threads when WorkerPool is constructed.

**Reasoning:**
- Avoids creating a new thread for every job.
- Reuses existing threads.
- Limits the number of concurrently executing jobs.
- Makes resource usage more predictable.

**Trade-off:** The current implementation does not dynamically increase or decrease the worker count.

## 10. Retry Mechanism

**Decision:** Retry failed jobs up to three attempts.

**Reasoning:**

Temporary failures can occur during task execution. Retrying provides an opportunity for a job to complete without requiring the application to resubmit it.

The current implementation demonstrates this with simulated failures.

**Trade-off:**
- Retrying increases total execution time.
- Retrying is not suitable for every kind of operation.
- Real-world systems need configurable retry policies and safeguards against repeating non-idempotent operations.

## 11. Graceful Shutdown

**Decision:** Provide an explicit `shutdown()` method and call it from the destructor.

**Reasoning:**

Worker threads must not outlive the WorkerPool object.

Shutdown:
1. Sets the stopping flag.
2. Notifies all workers.
3. Allows pending jobs to drain.
4. Joins worker threads.

**Trade-off:** The current implementation drains queued jobs rather than cancelling them. Concurrent calls to shutdown are not supported.

## 12. std::osyncstream for Logging

**Decision:** Use `std::osyncstream` for worker-thread console output.

**Reasoning:**

Multiple workers may write messages at approximately the same time.

Synchronized output prevents individual log messages from becoming interleaved.

**Trade-off:** Logging still has a performance cost, so it can be disabled during benchmarks.

## 13. Atomic Statistics

**Decision:** Use atomic counters for job statistics.

**Reasoning:**

Multiple workers can finish jobs and update statistics concurrently.

Atomic counters allow these updates without introducing a single statistics mutex.

Tracked metrics include:
- Submitted jobs.
- Completed jobs.
- Failed jobs.
- Total execution attempts.
- Retry attempts.
- Accumulated execution duration.

**Trade-off:** Individual atomic reads are safe, but reading multiple counters separately does not provide a guaranteed consistent snapshot while jobs are still running. Statistics should be inspected after shutdown when a final report is required.

## 14. Benchmark Design

**Decision:** Benchmark multiple worker counts using a simulated 10 ms job duration.

**Reasoning:**

The benchmark compares throughput and execution time with 1, 2, 4, and 8 workers.

It uses `std::chrono::steady_clock` because it measures elapsed time without depending on changes to the system clock.

Console logging and simulated failures are disabled to reduce measurement noise.

**Trade-off:** A sleep-based workload primarily demonstrates concurrent waiting-task processing. It does not establish CPU-bound parallel performance.

## 15. Future Design Improvements

Potential improvements include:

- Configurable retry policies with exponential backoff.
- Dynamic worker scaling.
- Job cancellation and deadlines.
- User-defined task callbacks.
- Persistent job storage.
- Lock-free queue experimentation.
- CPU-bound and mixed-workload benchmarks.
- Consistent statistics snapshots.
- More extensive stress and concurrency testing.

## 16. Conclusion

The design of FORGE prioritizes modularity, concurrency safety, predictable scheduling, and measurable performance.

The selected C++ standard library components provide a practical foundation for implementing a concurrent job scheduling engine without introducing unnecessary external dependencies.

The architecture also leaves room for future improvements as the project evolves from a functional prototype into a more general-purpose scheduling system.