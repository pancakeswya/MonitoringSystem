#ifndef MONITORINGSYSTEM_SRC_CONCURRENCY_WORKER_H_
#define MONITORINGSYSTEM_SRC_CONCURRENCY_WORKER_H_

#include <atomic>
#include <functional>

namespace monsys {

class Worker {
 public:
  using Work = std::function<void()>;
  Worker();
  ~Worker();
  void SetTimeout(unsigned int timeout) noexcept;
  void SetWork(Work work) noexcept;
  void Start();
 private:
  void DoWork();

  std::atomic<bool> working_, alive_;
  unsigned int timeout_;
  Work work_;
};

} // monsys

#endif // MONITORINGSYSTEM_SRC_CONCURRENCY_WORKER_H_
