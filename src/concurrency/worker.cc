#include "concurrency/worker.h"

#include <chrono>
#include <thread>

namespace monsys {

Worker::Worker() : working_(false), alive_(true), timeout_(1) {}

Worker::~Worker() {
  alive_ = false;
  while (working_)
    ;
}

void Worker::SetTimeout(unsigned int timeout) noexcept { timeout_ = timeout; }

void Worker::Start() { std::thread(&Worker::DoWork, this).detach(); }

void Worker::DoWork() {
  working_ = true;
  while (alive_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout_));
    work_();
  }
  working_ = false;
}

}  // namespace monsys
