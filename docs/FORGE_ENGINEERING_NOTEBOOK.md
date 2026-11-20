# FORGE — Engineering Notebook

## 1. Purpose

This notebook records the engineering concepts, implementation decisions, and technical lessons involved in developing FORGE.

The objective is not only to build a working job scheduling engine but also to understand the systems programming concepts behind it.

---

## 2. Understanding Job Scheduling

A job scheduling system manages units of work and determines when they should execute.

FORGE uses priority-based scheduling.

Each job contains:

- A unique identifier.
- A descriptive name.
- A priority value.
- An execution status.

Higher numeric priority values are selected before lower values.

For example:

| Job | Priority |
|---|---:|
| Payment Processing | 3 |
| Email Notification | 2 |
| Report Generation | 1 |

The payment job is selected first when all three are waiting in the queue.

---

## 3. Priority Queue Internals

FORGE uses the C++ Standard Library's `std::priority_queue`.

A priority queue is a heap-based data structure.

Important operations:

| Operation | Complexity |
|---|---:|
| Insert | O(log N) |
| Access highest priority | O(1) |
| Remove highest priority | O(log N) |

### Custom comparator

FORGE defines its own comparison logic to support:

1. Higher numeric priority first.
2. FIFO ordering for equal priorities.

A monotonically increasing sequence number is assigned to each submitted queue entry.

This provides deterministic tie-breaking within the queue.

---

## 4. Multithreading

Multithreading allows multiple threads to execute concurrently within one process.

FORGE uses `std::thread` to create worker threads.

Each worker repeatedly:

1. Waits for available work.
2. Retrieves a job.
3. Executes the job.
4. Updates its status.
5. Records statistics.
6. Returns to the waiting state.

The number of workers is configurable.

### Important distinction

Concurrency means multiple tasks can make progress during overlapping periods.

Parallelism means tasks are actually executing simultaneously on different processing resources.

The degree of parallel execution depends on available hardware and operating-system scheduling.

---

## 5. Mutex and Critical Sections

A mutex protects shared resources from simultaneous unsafe access.

FORGE uses `std::mutex` to protect:

- The shared job queue.
- The worker pool's stopping flag.

A critical section is the portion of code that accesses protected shared state.

The worker retrieves a job while holding the mutex, then releases the mutex before executing the job.

This is important because workers should not block each other while performing long-running tasks.

---

## 6. Condition Variables

A condition variable allows a thread to wait efficiently until a condition becomes true.

FORGE uses `std::condition_variable`.

Workers wait until:

- A job becomes available.
- The worker pool begins shutting down.

The worker uses a predicate with `condition.wait()`.

This is important because condition-variable waits can wake spuriously. The predicate ensures the worker checks the actual shared state before proceeding.

### Why not busy waiting?

Busy waiting repeatedly checks whether work is available.

This can waste CPU resources.

Condition variables allow idle workers to sleep until they receive notification.

---

## 7. Atomic Operations and Memory Ordering

FORGE uses atomic variables for job status and statistics counters.

An atomic operation prevents data races on the individual atomic variable.

For job status:

- Reads use acquire memory ordering.
- Writes use release memory ordering.

This provides synchronization around status publication and observation.

However, atomic status does not automatically protect unrelated fields such as job names or priorities.

Those fields are initialized before the job is shared and are not modified during execution.

---

## 8. Shared Ownership

FORGE uses `std::shared_ptr<Job>` to manage job lifetime.

Shared ownership is useful because the application and worker pool can hold references to the same job.

The job remains alive as long as at least one shared pointer owns it.

This avoids manually managing job memory and prevents workers from accessing a destroyed job when ownership is used correctly.

---

## 9. Retry Handling

Temporary execution failures are common in distributed and asynchronous systems.

FORGE demonstrates retry handling with a maximum of three attempts.

Example:

| Attempt | Result |
|---|---|
| 1 | Failed |
| 2 | Failed |
| 3 | Completed |

The current implementation simulates this behavior for job ID 103.

### Production considerations

A production scheduler should consider:

- Configurable retry limits.
- Exponential backoff.
- Maximum retry duration.
- Idempotent task execution.
- Permanent versus temporary failures.

Retrying an operation without considering idempotency can cause duplicate side effects.

---

## 10. Graceful Shutdown

Worker threads must be joined before the worker pool is destroyed.

FORGE implements graceful shutdown.

The process:

1. Sets the stopping flag.
2. Notifies all waiting workers.
3. Allows queued jobs to finish.
4. Joins all worker threads.

The destructor calls `shutdown()` as a cleanup safeguard.

The current implementation supports sequential repeated shutdown calls but does not support concurrent shutdown calls.

---

## 11. Thread-Safe Logging

Multiple workers may print messages simultaneously.

FORGE uses `std::osyncstream` to prevent individual log messages from being interleaved.

This makes concurrent execution output easier to read.

Logging can be disabled during benchmarking to reduce console overhead.

---

## 12. Performance Engineering

FORGE measures performance using:

- Execution time.
- Throughput.
- Speedup.
- Parallel efficiency.

### Execution time

The elapsed time between benchmark start and completion.

### Throughput

Number of completed jobs per second.

Formula:

Throughput = Completed Jobs / Execution Time in Seconds

### Speedup

Compares execution time against the single-worker baseline.

Formula:

Speedup = Single-worker Time / Current Execution Time

### Parallel efficiency

Formula:

Efficiency = Speedup / Number of Workers × 100

### Actual benchmark observations

| Workers | Time (ms) | Throughput |
|---:|---:|---:|
| 1 | 1836.82 | 54.44 jobs/sec |
| 2 | 970.44 | 103.05 jobs/sec |
| 4 | 469.36 | 213.06 jobs/sec |
| 8 | 215.28 | 464.51 jobs/sec |

The benchmark uses simulated waiting tasks. It demonstrates concurrent task processing rather than CPU-bound parallel performance.

---

## 13. Resource Management

FORGE uses automatic resource management through C++ objects and RAII principles.

Examples:

- `std::lock_guard` releases mutex locks automatically.
- `std::unique_lock` manages locks during condition-variable waits.
- `std::shared_ptr` manages shared job ownership.
- WorkerPool joins its threads during shutdown.

These mechanisms reduce the risk of resource leaks and forgotten cleanup operations.

---

## 14. Engineering Lessons

The development of FORGE demonstrates several important systems programming lessons:

1. Shared resources require synchronization.
2. A thread-safe status variable does not make an entire object thread-safe.
3. Locks should be held for as little time as practical.
4. Condition variables are preferable to busy waiting for idle workers.
5. Priority scheduling needs explicit tie-breaking rules when FIFO behavior is required.
6. Thread lifetime must be managed carefully.
7. Retry policies must account for repeated side effects.
8. Performance measurements need clearly defined workloads.
9. Passing functional tests does not prove the absence of all concurrency bugs.
10. Benchmark results should be reproducible and interpreted within their workload limitations.

---

## 15. Future Engineering Experiments

Possible experiments for future versions:

- Compare mutex-protected queues with alternative concurrent queue designs.
- Compare fixed-size and dynamically scaled worker pools.
- Introduce CPU-bound benchmark workloads.
- Implement exponential backoff.
- Add cancellation tokens.
- Measure queue waiting time separately from execution time.
- Measure lock contention.
- Perform high-volume concurrent producer testing.
- Explore thread sanitizers and other concurrency analysis tools.

---

## 16. Final Notes

FORGE provides practical experience with modern C++ and concurrent systems engineering.

The project combines scheduling algorithms, thread management, synchronization, atomic operations, error handling, resource management, automated testing, and benchmarking.

The engineering goal is to build a reliable foundation that can be extended into a more general-purpose job scheduling engine.