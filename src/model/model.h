#ifndef MONITORINGSYSTEM_SRC_MODEL_MODEL_H_
#define MONITORINGSYSTEM_SRC_MODEL_MODEL_H_

#include "agents/agents.h"
#include "agents/builder.h"
#include "agents/handler.h"
#include "base/types.h"
#include "model/config.h"

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>
#include <string>
#include <utility>

namespace monsys {

class Model {
 public:
  Model() noexcept;
  ~Model();

  AgentResponse LoadCpuAgent() noexcept;
  AgentResponse LoadMemoryAgent() noexcept;
  AgentResponse LoadNetworkAgent() noexcept;

  AgentResponse UnloadCpuAgent() noexcept;
  AgentResponse UnloadMemoryAgent() noexcept;
  AgentResponse UnloadNetworkAgent() noexcept;

  AgentResponse SetConfig(const std::string& config_path);

  std::vector<MetricResponse> UpdateMetrics();

  Metrics GetMetrics() noexcept;

  void Reset() noexcept;
 private:
  enum class ModelState {
    kIoBusy,
    kIoFree,
    kAboutToDestroy,
    kStoppedLogger
  };

  template<typename Callback>
  auto ExecuteAgent(Callback callback, const char* name);

  template<typename Tp>
  AgentStatus LoadAgent(Tp& agent);

  void LogMetrics(size_t delay);

  std::atomic<ModelState> state_;

  std::mutex mutex_;
  std::condition_variable cv_;

  agents::Builder builder_;
  agents::Handler handler_;

  agents::CPU cpu_agent_{};
  agents::Memory memory_agent_{};
  agents::Network network_agent_{};

  Metrics metrics_;

  SystemConfig config_{};
};

} // namespace monsys


#endif // MONITORINGSYSTEM_SRC_MODEL_MODEL_H_