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

  template<typename F, typename... Args>
  void SetWork(F&& f, Args... args);
  void SetTimeout(unsigned int timeout) noexcept;

  void Start();
 private:
  void DoWork();

  std::atomic<bool> working_, alive_;
  unsigned int timeout_;
  Work work_;
};

template<typename F, typename... Args>
inline void Worker::SetWork(F&& f, Args... args) {
  work_ = std::bind(f, args...);
}

} // monsys

#endif // MONITORINGSYSTEM_SRC_CONCURRENCY_WORKER_H_
