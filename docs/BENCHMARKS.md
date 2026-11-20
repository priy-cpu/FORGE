# FORGE Performance Benchmarks

## 1. Overview

FORGE is a C++20 priority-based job scheduling engine that uses a configurable worker pool to execute jobs concurrently.

This benchmark evaluates how execution time and throughput change when the worker count increases.

## 2. Benchmark Configuration

| Parameter | Value |
|---|---|
| Total jobs | 100 |
| Job duration | 10 ms |
| Worker configurations | 1, 2, 4, 8 |
| Simulated failures | Disabled |
| Console logging | Disabled |
| Compiler standard | C++20 |
| Optimization | `-O2 -DNDEBUG` |
| Workload | Simulated I/O-style tasks |

Each job simulates work using a 10 ms sleep. The benchmark measures the time required to submit and finish all 100 jobs.

## 3. Benchmark Results

The following results were measured on the development machine.

| Workers | Execution Time (ms) | Throughput (jobs/sec) | Speedup | Efficiency |
|---:|---:|---:|---:|---:|
| 1 | 1836.82 | 54.44 | 1.00x | 100.00% |
| 2 | 970.44 | 103.05 | 1.89x | 94.64% |
| 4 | 469.36 | 213.06 | 3.91x | 97.84% |
| 8 | 215.28 | 464.51 | 8.53x | 106.65% |

**Verification:** All 100 jobs completed successfully in every configuration.

## 4. Performance Analysis

### Execution time

Execution time decreased as the number of worker threads increased.

- 1 worker: 1836.82 ms
- 2 workers: 970.44 ms
- 4 workers: 469.36 ms
- 8 workers: 215.28 ms

The 8-worker configuration completed the workload approximately 8.53 times faster than the 1-worker baseline in this run.

### Throughput

Throughput represents the number of completed jobs per second.

The measured throughput increased from 54.44 jobs/sec with one worker to 464.51 jobs/sec with eight workers.

### Speedup

Speedup is calculated as:

`Speedup = Single-worker execution time / Current execution time`

### Parallel efficiency

Parallel efficiency is calculated as:

`Efficiency = (Speedup / Number of workers) * 100`

The measured efficiency for eight workers exceeds 100%. This can occur because the single-worker baseline and parallel runs are affected by scheduling, operating-system timing, and measurement overhead. It should not be interpreted as more than 100% physical CPU efficiency.

## 5. Benchmark Implementation

The benchmark uses:

- A configurable worker count.
- 100 independently submitted jobs.
- A 10 ms simulated job duration.
- Disabled logging to reduce console overhead.
- Disabled simulated failures to isolate normal execution.
- `std::chrono::steady_clock` for elapsed-time measurement.
- Job status verification after shutdown.

The benchmark verifies that every submitted job reaches `JobStatus::COMPLETED`.

## 6. Reproducing the Benchmark

Compile from the project root:

```bash
g++ -std=c++20 -O2 -DNDEBUG -Wall -Wextra -Wpedantic -pthread -Ischeduler/include benchmarks/worker_pool_benchmark.cpp scheduler/src/job.cpp scheduler/src/JobQueue.cpp scheduler/src/WorkerPool.cpp scheduler/src/JobStatistics.cpp -o forge_benchmark.exe
```

Run:

```bash
./forge_benchmark.exe
```

## 7. Limitations

- These measurements represent a single benchmark run.
- Results depend on hardware, operating-system scheduling, and background processes.
- The workload uses simulated waiting time rather than CPU-intensive computation.
- The benchmark demonstrates concurrent task processing, not CPU-bound parallel speedup.
- Repeated runs and averaged results would provide a more reliable performance comparison.

## 8. Conclusion

The benchmark demonstrates that FORGE can process a workload of 100 jobs using multiple worker threads, with all jobs completing successfully.

In the measured run, increasing the worker count from one to eight reduced execution time from 1836.82 ms to 215.28 ms.

This provides an initial performance demonstration of FORGE's concurrent job execution architecture.
