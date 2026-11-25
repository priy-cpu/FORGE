
FROM gcc:14

WORKDIR /app

COPY scheduler/ scheduler/
COPY tests/ tests/

# Build main FORGE application
RUN g++ -std=c++20 -Wall -Wextra -Wpedantic -pthread \
    -Ischeduler/include \
    scheduler/src/main.cpp \
    scheduler/src/job.cpp \
    scheduler/src/JobQueue.cpp \
    scheduler/src/JobStatistics.cpp \
    scheduler/src/WorkerPool.cpp \
    -o forge

# Build all seven test programs
RUN g++ -std=c++20 -pthread -Ischeduler/include \
    tests/priority_queue_test.cpp \
    scheduler/src/job.cpp scheduler/src/JobQueue.cpp \
    -o priority_test

RUN g++ -std=c++20 -pthread -Ischeduler/include \
    tests/worker_pool_test.cpp \
    scheduler/src/job.cpp scheduler/src/JobQueue.cpp \
    scheduler/src/WorkerPool.cpp scheduler/src/JobStatistics.cpp \
    -o worker_test

RUN g++ -std=c++20 -pthread -Ischeduler/include \
    tests/reliability_test.cpp \
    scheduler/src/job.cpp scheduler/src/JobQueue.cpp \
    scheduler/src/JobStatistics.cpp scheduler/src/WorkerPool.cpp \
    -o reliability_test

RUN g++ -std=c++20 -pthread -Ischeduler/include \
    tests/retry_exhaustion_test.cpp \
    scheduler/src/job.cpp scheduler/src/JobQueue.cpp \
    scheduler/src/JobStatistics.cpp scheduler/src/WorkerPool.cpp \
    -o retry_exhaustion_test

RUN g++ -std=c++20 -pthread -Ischeduler/include \
    tests/concurrent_submission_test.cpp \
    scheduler/src/job.cpp scheduler/src/JobQueue.cpp \
    scheduler/src/JobStatistics.cpp scheduler/src/WorkerPool.cpp \
    -o concurrent_submission_test

RUN g++ -std=c++20 -pthread -Ischeduler/include \
    tests/worker_lifecycle_test.cpp \
    scheduler/src/job.cpp scheduler/src/JobQueue.cpp \
    scheduler/src/JobStatistics.cpp scheduler/src/WorkerPool.cpp \
    -o worker_lifecycle_test

RUN g++ -std=c++20 -O2 -pthread -Ischeduler/include \
    tests/stress_test.cpp \
    scheduler/src/job.cpp scheduler/src/JobQueue.cpp \
    scheduler/src/JobStatistics.cpp scheduler/src/WorkerPool.cpp \
    -o stress_test

CMD ["./forge"]
