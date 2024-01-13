#ifndef MONITORINGSYSTEM_SRC_MODEL_MODEL_H_
#define MONITORINGSYSTEM_SRC_MODEL_MODEL_H_

#include "agents/agents.h"
#include "agents/builder.h"
#include "agents/handler.h"
#include "base/types.h"
#include "model/config.h"

#include <functional>
#include <string>
#include <utility>

namespace monsys {

class Model {
 public:
  Model() noexcept;

  AgentResponse LoadCpuAgent() noexcept;
  AgentResponse LoadMemoryAgent() noexcept;
  AgentResponse LoadNetworkAgent() noexcept;

  AgentResponse UnloadCpuAgent() noexcept;
  AgentResponse UnloadMemoryAgent() noexcept;
  AgentResponse UnloadNetworkAgent() noexcept;

  AgentResponse SetConfig(const std::string& config_path);

  MetricResponse UpdateMetrics();

  double CpuLoad() noexcept;
  size_t CpuProcesses() noexcept;

  double RamTotal() noexcept;
  double Ram() noexcept;
  double HardVolume() noexcept;
  size_t HardOps() noexcept;
  double HardThroughput() noexcept;

  double InetThroughput() noexcept;
  int UrlAvailable() noexcept;

  void Reset() noexcept;
 private:
  template<typename Callback>
  auto ExecuteAgent(Callback callback, const char* name);

  template<typename Tp>
  AgentStatus LoadAgent(Tp& agent);

  agents::Builder builder_;
  agents::Handler handler_;

  agents::CPU cpu_agent_{};
  agents::Memory memory_agent_{};
  agents::Network network_agent_{};

  double cpu_load_{};
  size_t cpu_processes_{};

  double ram_total_{};
  double ram_{};
  double hard_volume_{};
  size_t hard_ops_{};
  double hard_throughput_{};

  double inet_throughput_{};
  int url_available_{};

  SystemConfig config_{};
};

} // namespace monsys


#endif // MONITORINGSYSTEM_SRC_MODEL_MODEL_H_