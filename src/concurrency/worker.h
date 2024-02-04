#ifndef MONITORINGSYSTEM_SRC_CONCURRENCY_WORKER_H_
#define MONITORINGSYSTEM_SRC_CONCURRENCY_WORKER_H_

#include <atomic>
#include <functional>
#include <condition_variable>
#include <mutex>

namespace monsys {

using WorkCallback = std::function<void()>;

class Worker {
public:
    Worker();
    Worker(WorkCallback work) noexcept;
    ~Worker();
    void SetWork(WorkCallback work) noexcept;
    void Start();
private:
    void DoWork();

    std::atomic<bool> working_, alive_;
    std::mutex mutex_;
    std::condition_variable cv_;
    WorkCallback work_;
};

} // monsys

#endif // MONITORINGSYSTEM_SRC_CONCURRENCY_WORKER_H_
