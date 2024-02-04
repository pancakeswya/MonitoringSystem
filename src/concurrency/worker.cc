#include "concurrency/worker.h"

#include <thread>
#include <iostream>

namespace monsys {

Worker::Worker() : working_(false), alive_(true) {}

Worker::Worker(WorkCallback work) noexcept : Worker() {
    work_ = std::move(work);
}

Worker::~Worker() {
    alive_ = false;
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [&state = working_] { return state == false; });
}

void Worker::SetWork(WorkCallback work) noexcept {
    work_ = std::move(work);
}

void Worker::Start() {
    std::thread work_thread(&Worker::DoWork, this);
    work_thread.detach();
}

void Worker::DoWork() {
    while(alive_) {
        std::unique_lock<std::mutex> lock(mutex_);

        working_ = true;
        cv_.notify_one();

        work_();

        working_ = false;
        cv_.notify_one();
    }
}

} // namespace monsys
