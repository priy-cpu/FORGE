# FORGE
### A Concurrent Priority-Based Job Scheduling Engine in Modern C++

FORGE is a C++20 job scheduling engine designed to execute prioritized workloads concurrently using a configurable worker thread pool.

It features priority-based scheduling, FIFO ordering for equal-priority jobs, automatic retry handling, thread-safe job statistics, and graceful worker shutdown.

The project explores modern C++ concurrency, synchronization, scheduling algorithms, and performance engineering.

## Key Features

- **Priority-Based Scheduling:** Higher-priority jobs are selected before lower-priority jobs.
- **Concurrent Execution:** Configurable worker threads process jobs concurrently.
- **FIFO Tie-Breaking:** Jobs with equal priority preserve submission order in the queue.
- **Retry Mechanism:** Failed jobs can be retried up to three attempts.
- **Thread Safety:** Uses mutexes, condition variables, and atomic operations.
- **Graceful Shutdown:** Workers finish queued jobs before the pool shuts down.
- **Execution Statistics:** Tracks submitted jobs, completions, failures, attempts, and execution durations.
- **Performance Benchmarking:** Measures execution time, throughput, speedup, and efficiency.

## Architecture

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
                |  Priority   |
                | Scheduling  |
                +-------------+
                       |
              +--------+--------+
              |        |        |
              v        v        v
           Worker 1 Worker 2 Worker 3
              |        |        |
              +--------+--------+
                       |
                       v
                 Job Execution
                       |
                 +-----+-----+
                 |           |
                 v           v
             Job Status  JobStatistics
```

## Technology Stack

| Technology | Purpose |
|---|---|
| C++20 | Core implementation |
| `std::thread` | Concurrent worker execution |
| `std::mutex` | Shared resource synchronization |
| `std::condition_variable` | Worker coordination |
| `std::atomic` | Thread-safe status and counters |
| `std::priority_queue` | Priority-based scheduling |
| `std::shared_ptr` | Shared job ownership |
| `std::chrono` | Execution-time measurement |
| GCC / MinGW | Compilation |

## Project Structure

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

## Build and Run

### Prerequisites

- GCC with C++20 support
- Windows with MinGW, Linux, or another supported GCC environment

### Compile

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -pthread -Ischeduler/include scheduler/src/main.cpp scheduler/src/job.cpp scheduler/src/JobQueue.cpp scheduler/src/WorkerPool.cpp scheduler/src/JobStatistics.cpp -o forge
```

### Run

Windows:

```cmd
forge.exe
```

Linux:

```bash
./forge
```

## Performance Benchmark

FORGE was benchmarked with 100 jobs, each simulating 10 ms of work, using different worker counts.

| Workers | Execution Time | Throughput | Speedup |
|---:|---:|---:|---:|
| 1 | 1836.82 ms | 54.44 jobs/sec | 1.00x |
| 2 | 970.44 ms | 103.05 jobs/sec | 1.89x |
| 4 | 469.36 ms | 213.06 jobs/sec | 3.91x |
| 8 | 215.28 ms | 464.51 jobs/sec | 8.53x |

**Benchmark result:** All 100 jobs completed successfully in all four configurations.

The benchmark uses simulated waiting tasks. These measurements demonstrate concurrent task processing, not CPU-bound parallel performance. Results are from a single run and may vary across machines.

See [BENCHMARKS.md](docs/BENCHMARKS.md) for details.

## Testing

FORGE currently includes two automated test programs.

**Priority queue tests**
- Priority ordering.
- FIFO ordering for equal priorities.
- Empty queue behavior.

**Worker pool tests**
- Worker execution.
- Graceful shutdown.

Run the commands documented in [TESTING.md](docs/TESTING.md).

## Engineering Concepts Demonstrated

- Multithreaded programming in C++.
- Thread synchronization and mutual exclusion.
- Condition-variable-based worker coordination.
- Atomic operations and memory ordering.
- Heap-based priority scheduling.
- Shared ownership and RAII.
- Retry and failure handling.
- Performance benchmarking and throughput analysis.

## Future Improvements

- User-defined task callbacks.
- Dynamic worker scaling.
- Job cancellation and deadlines.
- Configurable retry policies with exponential backoff.
- Persistent job storage.
- Distributed job execution.
- CPU-intensive performance benchmarks.
- Expanded concurrency and stress testing.

## Documentation

- [System Architecture](docs/ARCHITECTURE.md)
- [Design Decisions](docs/DESIGN_DECISIONS.md)
- [Benchmark Results](docs/BENCHMARKS.md)
- [Testing Guide](docs/TESTING.md)
- [Implementation Log](docs/IMPLEMENTATION_LOG.md)
- [Engineering Notebook](docs/FORGE_ENGINEERING_NOTEBOOK.md)

## Author

Developed as a systems programming project to explore modern C++, concurrent execution, and job scheduling architecture.

---

**FORGE — Building a foundation for concurrent job execution.**