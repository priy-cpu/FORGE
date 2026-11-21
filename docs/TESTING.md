# FORGE — Testing and Validation

## 1. Overview

This document describes the testing strategy, compilation commands, and validation results for FORGE.

FORGE includes seven automated test programs covering priority scheduling, worker execution, reliability, retry handling, concurrent submissions, worker lifecycle management, and large-scale stress testing.

A separate performance benchmark evaluates execution time and throughput under different worker configurations.

## 2. Testing Environment

| Component | Configuration |
|---|---|
| Language | C++20 |
| Compiler | GCC / MinGW g++ |
| Build standard | `-std=c++20` |
| Thread support | `-pthread` |
| Warning flags | `-Wall -Wextra -Wpedantic` |
| Optimization | `-O2` for stress and benchmark builds |
| Operating system | Windows |

All test programs were compiled using GCC with C++20 support.

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
g++ -std=c++20 -Wall -Wextra -Wpedantic -pthread -Ischeduler/include tests/priority_queue_test.cpp scheduler/src/job.cpp scheduler/src/JobQueue.cpp -o priority_test.exe
```

### Run

```cmd
.\priority_test.exe
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

- Worker thread execution.
- Job processing.
- Graceful shutdown.
- Correct worker pool behavior.

### Compile

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -pthread -Ischeduler/include tests/worker_pool_test.cpp scheduler/src/job.cpp scheduler/src/JobQueue.cpp scheduler/src/WorkerPool.cpp scheduler/src/JobStatistics.cpp -o worker_test.exe
```

### Run

```cmd
.\worker_test.exe
```

### Verified result

```text
All WorkerPool tests passed!
```

## 5. Reliability Tests

**File:** `tests/reliability_test.cpp`

### Objective

Verify that WorkerPool handles invalid operations safely.

### Test cases

1. Creating a worker pool with zero workers is rejected.
2. Submitting a null job is rejected.
3. Submitting a job after shutdown is rejected.

### Compile

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -pthread -Ischeduler/include tests/reliability_test.cpp scheduler/src/job.cpp scheduler/src/JobQueue.cpp scheduler/src/JobStatistics.cpp scheduler/src/WorkerPool.cpp -o reliability_test.exe
```

### Run

```cmd
.\reliability_test.exe
```

### Verified result

```text
PASS: Zero workers rejected
PASS: Null job rejected
PASS: Submission after shutdown rejected

All reliability tests passed!
```

## 6. Retry Exhaustion Tests

**File:** `tests/retry_exhaustion_test.cpp`

### Objective

Verify that permanently failing jobs are retried only up to the configured maximum number of attempts and are eventually marked as failed.

### Test coverage

- Retry handling.
- Maximum retry attempts.
- Permanent job failure.
- Correct FAILED job status.
- Worker pool shutdown after failure.

### Compile

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -pthread -Ischeduler/include tests/retry_exhaustion_test.cpp scheduler/src/job.cpp scheduler/src/JobQueue.cpp scheduler/src/JobStatistics.cpp scheduler/src/WorkerPool.cpp -o retry_exhaustion_test.exe
```

### Run

```cmd
.\retry_exhaustion_test.exe
```

### Verified result

```text
PASS: Permanently failing job marked FAILED
PASS: WorkerPool completed shutdown
All retry exhaustion tests passed!
```

## 7. Concurrent Submission Tests

**File:** `tests/concurrent_submission_test.cpp`

### Objective

Verify that multiple producer threads can submit jobs concurrently without losing jobs or corrupting execution statistics.

### Configuration

- 4 producer threads.
- 4 worker threads.
- 2,500 jobs per producer.
- 10,000 total jobs.
- Simulated failures disabled.

### Compile

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -pthread -Ischeduler/include tests/concurrent_submission_test.cpp scheduler/src/job.cpp scheduler/src/JobQueue.cpp scheduler/src/JobStatistics.cpp scheduler/src/WorkerPool.cpp -o concurrent_submission_test.exe
```

### Run

```cmd
.\concurrent_submission_test.exe
```

### Verified results

| Metric | Expected | Actual |
|---|---:|---:|
| Submitted | 10,000 | 10,000 |
| Completed | 10,000 | 10,000 |
| Failed | 0 | 0 |
| Total attempts | 10,000 | 10,000 |
| Retry attempts | 0 | 0 |

```text
PASS: All 10,000 jobs completed!
PASS: All statistics verified!
```

## 8. Worker Lifecycle Tests

**File:** `tests/worker_lifecycle_test.cpp`

### Objective

Verify that WorkerPool handles repeated shutdown requests, concurrent shutdown calls, and automatic shutdown during destruction.

### Test coverage

- 50 repeated worker pool lifecycle cycles.
- 5,000 jobs processed across lifecycle cycles.
- Two threads calling shutdown concurrently.
- Repeated shutdown calls.
- Automatic shutdown through the destructor.
- No lost or incomplete jobs.

### Compile

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -pthread -Ischeduler/include tests/worker_lifecycle_test.cpp scheduler/src/job.cpp scheduler/src/JobQueue.cpp scheduler/src/JobStatistics.cpp scheduler/src/WorkerPool.cpp -o worker_lifecycle_test.exe
```

### Run

```cmd
.\worker_lifecycle_test.exe
```

### Verified result

```text
PASS: Repeated shutdown works!
PASS: Concurrent shutdown requests work!
PASS: Automatic destructor shutdown works!
All lifecycle tests passed!
```

## 9. Large-Scale Stress Test

**File:** `tests/stress_test.cpp`

### Objective

Evaluate job processing reliability under a large concurrent workload.

The stress test submits 100,000 jobs using multiple producer threads while worker threads process the shared job queue.

### Configuration

| Parameter | Value |
|---|---:|
| Producer threads | 8 |
| Worker threads | 8 |
| Jobs per producer | 12,500 |
| Total jobs | 100,000 |
| Simulated failures | Disabled |
| Job duration | 0 ms |

### Compile

```bash
g++ -std=c++20 -O2 -Wall -Wextra -Wpedantic -pthread -Ischeduler/include tests/stress_test.cpp scheduler/src/job.cpp scheduler/src/JobQueue.cpp scheduler/src/JobStatistics.cpp scheduler/src/WorkerPool.cpp -o stress_test.exe
```

### Run

```cmd
.\stress_test.exe
```

### Verified results

| Metric | Result |
|---|---:|
| Expected jobs | 100,000 |
| Submitted | 100,000 |
| Completed | 100,000 |
| Failed | 0 |
| Total attempts | 100,000 |
| Retry attempts | 0 |
| Individually completed | 100,000 |
| Individually failed | 0 |
| Lost or incomplete jobs | 0 |

### Final result

```text
PASS: All 100,000 jobs completed!
PASS: Statistics verified!
PASS: No jobs lost or left incomplete!
```

The test successfully processed all 100,000 jobs and verified the execution statistics.

## 10. Performance Benchmark

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

```cmd
.\forge_benchmark.exe
```

### Measured results

| Workers | Execution Time (ms) | Throughput (jobs/sec) | Speedup |
|---:|---:|---:|---:|
| 1 | 1836.82 | 54.44 | 1.00x |
| 2 | 970.44 | 103.05 | 1.89x |
| 4 | 469.36 | 213.06 | 3.91x |
| 8 | 215.28 | 464.51 | 8.53x |

All 100 jobs completed successfully in all four configurations.

**Note:** The benchmark uses simulated waiting tasks. These measurements demonstrate concurrent task processing, not CPU-bound parallel performance. Results are from a single run and may vary across machines.

## 11. Overall Validation Summary

| Test Program | Workload / Coverage | Result |
|---|---|---|
| Priority queue tests | Priority and FIFO ordering | PASS |
| Worker pool tests | Execution and shutdown | PASS |
| Reliability tests | Invalid operations | PASS |
| Retry exhaustion tests | Maximum retry handling | PASS |
| Concurrent submission tests | 10,000 jobs | PASS |
| Worker lifecycle tests | Repeated and concurrent shutdown | PASS |
| Stress tests | 100,000 jobs | PASS |
| Performance benchmark | 1, 2, 4, and 8 workers | Completed |

All seven automated test programs passed their respective test runs.

## 12. Conclusion

The testing process validates the core scheduling, execution, retry, statistics, and worker lifecycle behavior of FORGE.

The 100,000-job stress test provides additional evidence that concurrent job submission and processing work correctly under a substantial workload.

These results establish a strong testing foundation for future improvements, including dynamic worker scaling, job cancellation, persistent scheduling, and distributed execution.