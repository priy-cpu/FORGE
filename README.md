# FORGE

### A Concurrent Priority-Based Job Scheduling Engine in Modern C++

FORGE is a C++20 job scheduling engine designed to execute prioritized workloads concurrently using a configurable worker thread pool.

It features priority-based scheduling, FIFO ordering for equal-priority jobs, automatic retry handling, thread-safe job statistics, graceful worker shutdown, and concurrent job submission.

The project explores modern C++ concurrency, synchronization, scheduling algorithms, reliability engineering, and performance analysis.

## Key Features

- **Priority-Based Scheduling:** Higher-priority jobs are selected before lower-priority jobs.
- **Concurrent Execution:** Configurable worker threads process jobs concurrently.
- **FIFO Tie-Breaking:** Jobs with equal priority preserve submission order.
- **Retry Mechanism:** Failed jobs can be retried up to three attempts.
- **Thread Safety:** Uses mutexes, condition variables, and atomic operations.
- **Graceful Shutdown:** Workers finish queued jobs before the pool shuts down.
- **Concurrent Shutdown:** Multiple shutdown requests are safely coordinated.
- **Execution Statistics:** Tracks submitted jobs, completions, failures, attempts, and execution durations.
- **Reliability Testing:** Covers invalid inputs, retry exhaustion, concurrent submissions, and worker lifecycle.
- **Stress Testing:** Successfully processed 100,000 jobs using eight producer threads and eight worker threads.
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
                |   Priority  |
                |  Scheduling |
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
                +------+------+
                |             |
                v             v
            Job Status   JobStatistics
```

## Technology Stack

| Technology | Purpose |
|---|---|
| C++20 | Core implementation |
| `std::thread` | Concurrent worker execution |
| `std::mutex` | Shared resource synchronization |
| `std::condition_variable` | Worker coordination |
| `std::atomic` | Thread-safe counters |
| `std::priority_queue` | Priority-based scheduling |
| `std::shared_ptr` | Shared job ownership |
| `std::chrono` | Execution-time measurement |
| GCC / MinGW | Compilation |

## Project Structure

```text
FORGE/
|
+-- benchmarks/
|   +-- worker_pool_benchmark.cpp
|
+-- docs/
|   +-- ARCHITECTURE.md
|   +-- BENCHMARKS.md
|   +-- DESIGN_DECISIONS.md
|   +-- FORGE_ENGINEERING_NOTEBOOK.md
|   +-- IMPLEMENTATION_LOG.md
|   +-- TESTING.md
|
+-- scheduler/
|   +-- include/
|   |   +-- Job.h
|   |   +-- JobQueue.h
|   |   +-- JobStatistics.h
|   |   +-- WorkerPool.h
|   |
|   +-- src/
|       +-- job.cpp
|       +-- JobQueue.cpp
|       +-- JobStatistics.cpp
|       +-- main.cpp
|       +-- WorkerPool.cpp
|
+-- tests/
|   +-- priority_queue_test.cpp
|   +-- worker_pool_test.cpp
|   +-- reliability_test.cpp
|   +-- retry_exhaustion_test.cpp
|   +-- concurrent_submission_test.cpp
|   +-- worker_lifecycle_test.cpp
|   +-- stress_test.cpp
+-- .gitignore
+-- LICENSE
+-- Dockerfile
+-- Dockerfile.prod
+-- compose.yaml
+-- README.md
```

## Build and Run

### Prerequisites

- GCC with C++20 support
- Windows with MinGW, Linux, or another supported GCC environment

### Compile

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -pthread -Ischeduler/include scheduler/src/main.cpp scheduler/src/job.cpp scheduler/src/JobQueue.cpp scheduler/src/JobStatistics.cpp scheduler/src/WorkerPool.cpp -o forge
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



## Docker and Containerization

FORGE supports containerized development, testing, and production execution using Docker.

### Docker Images

The project uses two Docker images:

| Image | Purpose |
|---|---|
| `forge:phase3` | Development image containing the application and seven test executables |
| `forge:production` | Lightweight production image containing only the compiled application |

### Build Development Image

```bash
docker build -t forge:phase3 .
```

### Run All Tests Inside Docker

```bash
docker run --rm forge:phase3 sh -c "./priority_test && ./worker_test && ./reliability_test && ./retry_exhaustion_test && ./concurrent_submission_test && ./worker_lifecycle_test && ./stress_test"
```

All seven test programs passed inside the Linux container, including the 100,000-job stress test.

### Build Production Image

```bash
docker build -f Dockerfile.prod -t forge:production .
```

### Run Production Container

```bash
docker run --rm forge:production
```

### Docker Compose

Build and start FORGE using Docker Compose:

```bash
docker compose up --build
```

Stop and remove the container and network:

```bash
docker compose down
```

### Multi-Stage Build Optimization

The production image uses a multi-stage Docker build:

- Builder stage: GCC 14 compiles the C++20 application.
- Runtime stage: Debian 13 Slim contains only the compiled executable and runtime dependencies.

| Image | Reported Disk Usage |
|---|---:|
| Development image | 2.14 GB |
| Production image | 118 MB |

**Result:** Approximately 94.5% reduction in reported image disk usage.

The production container was successfully executed, and all six demonstration jobs completed with zero failures.

Docker validation was performed on Docker Desktop using Linux containers.




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

## Reliability and Testing

FORGE includes seven test programs covering scheduling, execution, reliability, and concurrency.

| Test | Coverage |
|---|---|
| `priority_queue_test.cpp` | Priority ordering, FIFO tie-breaking, empty queue |
| `worker_pool_test.cpp` | Worker execution and graceful shutdown |
| `reliability_test.cpp` | Zero workers, null jobs, submission after shutdown |
| `retry_exhaustion_test.cpp` | Retry exhaustion and failed job status |
| `concurrent_submission_test.cpp` | 10,000 jobs submitted by concurrent producers |
| `worker_lifecycle_test.cpp` | Repeated shutdown, concurrent shutdown, destructor shutdown |
| `stress_test.cpp` | 100,000 jobs with eight producers and eight workers |

### Stress Test Results

The 100,000-job stress test completed successfully.

| Metric | Result |
|---|---:|
| Producer threads | 8 |
| Worker threads | 8 |
| Jobs submitted | 100,000 |
| Jobs completed | 100,000 |
| Jobs failed | 0 |
| Total attempts | 100,000 |
| Retry attempts | 0 |
| Lost or incomplete jobs | 0 |

**Result:** All 100,000 jobs completed, and all statistics were verified successfully.

All seven test programs have passed their respective test runs.

See [TESTING.md](docs/TESTING.md) for compilation and execution instructions.

## Engineering Concepts Demonstrated

- Multithreaded programming in modern C++.
- Thread synchronization and mutual exclusion.
- Condition-variable-based worker coordination.
- Atomic operations.
- Heap-based priority scheduling.
- Shared ownership and RAII.
- Retry and failure handling.
- Graceful worker lifecycle management.
- Concurrent producer workloads.
- Stress testing and performance analysis.

## Future Improvements

- User-defined task callbacks.
- Dynamic worker scaling.
- Job cancellation and deadlines.
- Configurable retry policies with exponential backoff.
- Persistent job storage.
- Distributed job execution.
- CPU-intensive performance benchmarks.
- Advanced scheduling policies.
- Monitoring and observability.

## Documentation

- [System Architecture](docs/ARCHITECTURE.md)
- [Design Decisions](docs/DESIGN_DECISIONS.md)
- [Benchmark Results](docs/BENCHMARKS.md)
- [Testing Guide](docs/TESTING.md)
- [Implementation Log](docs/IMPLEMENTATION_LOG.md)
- [Engineering Notebook](docs/FORGE_ENGINEERING_NOTEBOOK.md)

## Author

Developed as a systems programming project to explore modern C++, concurrent execution, reliability engineering, and job scheduling architecture.

---

**FORGE — Building a foundation for concurrent job execution.**