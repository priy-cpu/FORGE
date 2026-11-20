#include <cassert>
#include <iostream>
#include <memory>

#include "JobQueue.h"

int main() {
    forge::JobQueue queue;

    auto job1 = std::make_shared<forge::Job>(1, "Invoice", 1);
    auto job2 = std::make_shared<forge::Job>(2, "Payment", 3);
    auto job3 = std::make_shared<forge::Job>(3, "Email", 2);
    auto job4 = std::make_shared<forge::Job>(4, "Backup", 3);
    auto job5 = std::make_shared<forge::Job>(5, "Report", 1);

    queue.enqueue(job1);
    queue.enqueue(job2);
    queue.enqueue(job3);
    queue.enqueue(job4);
    queue.enqueue(job5);

    // Highest priority first.
    assert(queue.dequeue()->getId() == 2);

    // Equal priority: FIFO ordering.
    assert(queue.dequeue()->getId() == 4);

    assert(queue.dequeue()->getId() == 3);

    assert(queue.dequeue()->getId() == 1);

    assert(queue.dequeue()->getId() == 5);

    assert(queue.isEmpty());
    assert(queue.size() == 0);
    assert(queue.dequeue() == nullptr);

    std::cout << "All priority queue tests passed!\n";

    return 0;
}
