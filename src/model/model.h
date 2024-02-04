#ifndef MONITORINGSYSTEM_SRC_MODEL_MODEL_H_
#define MONITORINGSYSTEM_SRC_MODEL_MODEL_H_

#include "agents/agents.h"
#include "agents/builder.h"
#include "agents/handler.h"
#include "base/types.h"

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

  void SetExceptionCallback(ExceptionCallback callback) noexcept;

  void LoadAgents();
  void UpdateMetrics();
  void UpdateConfig(const SystemConfig& config);
  SystemConfig GetConfig() noexcept;

  Metrics GetMetrics() noexcept;
 private:
  enum class State {
    kIoBusy,
    kIoFree,
    kLoadTerminate,
    kLoggerTerminate,
    kLoadStopped,
    kLoggerStopped
  };

  template<typename Callback>
  auto ExecuteAgent(Callback callback, const char* name);

  template<typename Tp>
  AgentResponse LoadAgent(Tp& agent, const char* name);

  AgentResponse SetConfig(const std::string& config_path);

  std::vector<AgentResponse> LoadAgents_();
  std::vector<MetricResponse> UpdateMetrics_();

  void LoadAgentsWithDelay(size_t delay);
  void LogMetrics(size_t delay);

  void HandleAgentResponse(const AgentResponse& response);
  void HandleMetricsResponse(const MetricResponse& response);

  std::atomic<State> state_;

  std::mutex mutex_;
  std::condition_variable cv_;

  agents::Builder builder_;
  agents::Handler handler_;

  agents::CPU cpu_agent_{};
  agents::Memory memory_agent_{};
  agents::Network network_agent_{};

  Metrics metrics_{};

  SystemConfig config_{};
  ExceptionCallback exception_callback_;
};

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_MODEL_MODEL_H_
