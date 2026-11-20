# FORGE — Testing and Validation

## 1. Overview

This document describes the testing strategy, compilation commands, and validation results for FORGE.

The project currently includes automated tests for priority queue behavior and worker pool functionality, along with a performance benchmark.

## 2. Testing Environment

| Component | Configuration |
|---|---|
| Language | C++20 |
| Compiler | GCC / MinGW g++ |
| Build standard | `-std=c++20` |
| Thread support | `-pthread` |
| Warning flags | `-Wall -Wextra -Wpedantic` |
| Operating system | Windows |

## 3. Priority Queue Tests

**File:** `tests/priority_queue_test.cpp`

### Objective

Verify that JobQueue correctly implements priority-based scheduling and FIFO ordering for equal priorities.

### Test cases

1. Higher-priority jobs are dequeued before lower-priority jobs.
2. Jobs with equal priorities preserve FIFO ordering.
3. An empty queue reports that it is empty.
4. Dequeuing from an empty queue returns a null pointer.

### Compile

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -Ischeduler/include tests/priority_queue_test.cpp scheduler/src/job.cpp scheduler/src/JobQueue.cpp -o priority_test.exe
```

### Run

```bash
./priority_test.exe
```

### Verified result

```text
All priority queue tests passed!
```

## 4. Worker Pool Tests

**File:** `tests/worker_pool_test.cpp`

### Objective

Verify the core functionality of WorkerPool.

### Test coverage

The current test suite verifies worker pool behavior, including job execution and shutdown.

### Compile

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -pthread -Ischeduler/include tests/worker_pool_test.cpp scheduler/src/job.cpp scheduler/src/JobQueue.cpp scheduler/src/WorkerPool.cpp scheduler/src/JobStatistics.cpp -o worker_test.exe
```

### Run

```bash
./worker_test.exe
```

### Verified result

```text
Worker 1 executing: Shutdown Test | Attempt: 1
Job 201 completed successfully!
All WorkerPool tests passed!
```

## 5. Performance Benchmark

**File:** `benchmarks/worker_pool_benchmark.cpp`

### Objective

Evaluate execution time, throughput, speedup, and parallel efficiency with different worker counts.

### Configuration

- 100 jobs.
- 10 ms simulated job duration.
- Worker counts: 1, 2, 4, and 8.
- Simulated failures disabled.
- Console logging disabled.

### Compile

```bash
g++ -std=c++20 -O2 -DNDEBUG -Wall -Wextra -Wpedantic -pthread -Ischeduler/include benchmarks/worker_pool_benchmark.cpp scheduler/src/job.cpp scheduler/src/JobQueue.cpp scheduler/src/WorkerPool.cpp scheduler/src/JobStatistics.cpp -o forge_benchmark.exe
```

### Run

```bash
./forge_benchmark.exe
```

### Measured results

| Workers | Execution Time (ms) | Throughput (jobs/sec) | Speedup | Efficiency |
|---:|---:|---:|---:|---:|
| 1 | 1836.82 | 54.44 | 1.00x | 100.00% |
| 2 | 970.44 | 103.05 | 1.89x | 94