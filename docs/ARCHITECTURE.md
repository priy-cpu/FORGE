# FORGE — System Architecture

## 1. Introduction

FORGE is a C++20 priority-based job scheduling engine designed to execute multiple jobs concurrently using a configurable worker thread pool.

The system supports:

- Priority-based job scheduling.
- Multiple worker threads.
- FIFO ordering for jobs with equal priority.
- Thread-safe job status management.
- Automatic retry handling.
- Graceful worker pool shutdown.
- Execution statistics and performance benchmarking.

## 2. Technology Stack

| Component | Technology |
|---|---|
| Programming language | C++20 |
| Concurrency | `std::thread` |
| Synchronization | `std::mutex`, `std::condition_variable` |
| Atomic operations | `std::atomic` |
| Job scheduling | `std::priority_queue` |
| Shared ownership | `std::shared_ptr` |
| Thread-safe logging | `std::osyncstream` |
| Timing | `std::chrono` |
| Build | GCC / MinGW |

## 3. High-Level Architecture

FORGE consists of four primary components:

1. **Job** — Represents an individual unit of work.
2. **JobQueue** — Stores pending jobs according to priority.
3. **WorkerPool** — Manages worker threads and job execution.
4. **JobStatistics** — Collects execution and retry metrics.

```text
                  APPLICATION
                       |
                       v
                  submit(Job)
                       |
                       v
                +-------------+
                |  WorkerPool |
                +-------------+
                       |
                       v
                +-------------+
                |   JobQueue  |
                | Priority    |
                | Scheduling  |
                +-------------+
                       |
                 Worker Threads
                  /    |    \
                 v     v     v
              Worker Worker Worker
                 |     |     |
                 +-----+-----+
                       |
                       v
                 Job Execution
                       |
                +------+------+
                |             |
                v             v
             Job Status   JobStatistics
```

## 4. Component Design

### 4.1 Job

Files:

- `scheduler/include/Job.h`
- `scheduler/src/job.cpp`

The Job class represents an individual task.

Each job contains:

| Field | Description |
|---|---|
| ID | Unique identifier supplied by the application |
| Name | Descriptive job name |
| Priority | Numeric scheduling priority |
| Status | Current execution state |

Supported job statuses:

- `PENDING`
- `RUNNING`
- `COMPLETED`
- `FAILED`

The job status is maintained using `std::atomic<JobStatus>` to allow safe concurrent status reads and updates.

Job IDs, names, and priorities are initialized during construction and are not modified during execution.

### 4.2 JobQueue

Files:

- `scheduler/include/JobQueue.h`
- `scheduler/src/JobQueue.cpp`

JobQueue stores pending jobs using `std::priority_queue`.

Scheduling rules:

1. Jobs with higher numeric priority execute first.
2. Jobs with equal priority follow FIFO ordering.
3. Each queue entry contains a sequence number to preserve submission order for equal priorities.

Example:

| Job | Priority | Expected queue order |
|---|---:|---:|
| Job A | 1 | 4 |
| Job B | 3 | 1 |
| Job C | 2 | 3 |
| Job D | 3 | 2 |

The queue itself is not independently thread-safe. WorkerPool protects queue operations using its mutex.

### 4.3 WorkerPool

Files:

- `scheduler/include/WorkerPool.h`
- `scheduler/src/WorkerPool.cpp`

WorkerPool is responsible for concurrent job execution.

Responsibilities:

- Create and manage worker threads.
- Accept submitted jobs.
- Coordinate access to the shared job queue.
- Wake workers when jobs become available.
- Execute jobs.
- Retry failed jobs.
- Maintain job statistics.
- Shut down workers gracefully.

#### Worker execution lifecycle

1. A worker waits on a condition variable.
2. When a job becomes available, the worker acquires the queue mutex.
3. The worker removes the highest-priority available job.
4. The worker releases the mutex.
5. The worker executes the job.
6. A failed job is retried up to the configured maximum of three attempts.
7. The job is marked `COMPLETED` or `FAILED`.
8. Execution statistics are updated.

#### Synchronization

| Mechanism | Purpose |
|---|---|
| `std::mutex` | Protect shared queue and shutdown state |
| `std::condition_variable` | Wake idle workers when work becomes available |
| `std::atomic` | Protect job status and statistics counters |
| `std::osyncstream` | Prevent interleaved console output |

The worker threads do not hold the queue mutex while performing simulated job execution. This allows other workers to retrieve jobs while a job is running.

#### Shutdown

WorkerPool supports explicit shutdown through `shutdown()`.

Shutdown:

1. Sets the stopping flag.
2. Notifies all waiting workers.
3. Allows queued jobs to finish.
4. Joins worker threads before returning.

The destructor also calls shutdown to ensure worker threads are joined when the pool is destroyed.

Explicit shutdown is designed for sequential use; concurrent calls to shutdown are not supported.

### 4.4 JobStatistics

Files:

- `scheduler/include/JobStatistics.h`
- `scheduler/src/JobStatistics.cpp`

JobStatistics maintains atomic counters and execution-time measurements.

Tracked metrics:

| Metric | Description |
|---|---|
| Submitted jobs | Total accepted jobs |
| Completed jobs | Jobs completed successfully |
| Failed jobs | Jobs that exhausted their retry attempts |
| Total attempts | Total execution attempts |
| Retry attempts | Attempts after the initial attempt |
| Total execution time | Sum of execution durations across finished jobs |
| Average execution time | Average duration per finished job |

The execution-time metrics represent accumulated per-job execution durations, not total wall-clock time for the entire workload.

## 5. Job Lifecycle

```text
        +---------+
        | PENDING |
        +---------+
             |
             v
        +---------+
        | RUNNING |
        +---------+
             |
          +--+--+
          |     |
          v     v
    +-----------+  +--------+
    | COMPLETED |  | FAILED |
    +-----------+  +--------+
```

A job can move from `RUNNING` back to `RUNNING` when another execution attempt begins after a failure.

After the maximum of three attempts:

- Successful execution results in `COMPLETED`.
- Exhausted retries result in `FAILED`.

## 6. Project Structure

```text
FORGE/
│
├── benchmarks/
│   └── worker_pool_benchmark.cpp
│
├── docs/
│   ├── ARCHITECTURE.md
│   ├── BENCHMARKS.md
│   ├── DESIGN_DECISIONS.md
│   ├── FORGE_ENGINEERING_NOTEBOOK.md
│   ├── IMPLEMENTATION_LOG.md
│   └── TESTING.md
│
├── scheduler/
│   ├── include/
│   │   ├── Job.h
│   │   ├── JobQueue.h
│   │   ├── JobStatistics.h
│   │   └── WorkerPool.h
│   │
│   └── src/
│       ├── job.cpp
│       ├── JobQueue.cpp
│       ├── JobStatistics.cpp
│       ├── main.cpp
│       └── WorkerPool.cpp
│
├── tests/
│   ├── priority_queue_test.cpp
│   └── worker_pool_test.cpp
│
├── .gitignore
├── LICENSE
└── README.md
```

## 7. Design Principles

FORGE follows these engineering principles:

- **Separation of concerns:** Job representation, scheduling, execution, and statistics are implemented independently.
- **Concurrency safety:** Shared queue access and job status updates use appropriate synchronization.
- **Resource management:** Worker threads are joined during shutdown and destruction.
- **Deterministic priority ordering:** Sequence numbers preserve FIFO ordering for equal priorities.
- **Configurable execution:** Worker count, simulated duration, logging, and failure simulation can be configured.
- **Measurable performance:** Benchmarks track execution time, throughput, speedup, and efficiency.

## 8. Current Scope and Limitations

The current implementation is a functional prototype.

It currently uses simulated job execution rather than a general-purpose user-defined task execution interface.

Additional capabilities that could be developed in future versions include:

- Dynamic worker scaling.
- Job cancellation.
- Job deadlines and timeouts.
- Persistent job storage.
- Configurable retry policies and backoff.
- User-defined task callbacks.
- CPU-intensive benchmark workloads.
- More extensive automated concurrency testing.

## 9. Conclusion

FORGE demonstrates the core architecture of a concurrent priority-based job scheduling engine in modern C++.

Its design combines priority scheduling, worker thread coordination, retry handling, atomic statistics, and graceful shutdown into a modular system.

The project provides a foundation for exploring operating-system concurrency, synchronization, scheduling algorithms, and performance engineering.