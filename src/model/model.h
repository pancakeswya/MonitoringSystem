#ifndef MONITORINGSYSTEM_SRC_MODEL_MODEL_H_
#define MONITORINGSYSTEM_SRC_MODEL_MODEL_H_

#include "agents/agents.h"
#include "agents/builder.h"
#include "agents/handler.h"
#include "model/config.h"

#include <string>
#include <utility>

namespace monsys {

enum class MetricStatus {
  kOutOfRange,
  kInvalidUrl,
  kOk
};

class Model {
 public:
  Model() noexcept;

  agents::AgentStatus LoadCpuAgent() noexcept;
  agents::AgentStatus LoadMemoryAgent() noexcept;
  agents::AgentStatus LoadNetworkAgent() noexcept;

  agents::AgentStatus SetConfig(const std::string& config_path);

  std::pair<MetricStatus, double> CpuLoad() noexcept;
  std::pair<MetricStatus, size_t> CpuProcesses() noexcept;

  std::pair<MetricStatus, double> RamTotal() noexcept;
  std::pair<MetricStatus, double> Ram() noexcept;
  std::pair<MetricStatus, double> HardVolume() noexcept;
  std::pair<MetricStatus, size_t> HardOps() noexcept;
  std::pair<MetricStatus, double> HardThroughput() noexcept;

  std::pair<MetricStatus, double> InetThroughput() noexcept;
  std::pair<MetricStatus, int> UrlAvailable() noexcept;

  void Reset() noexcept;
 private:
  agents::Builder builder_;
  agents::Handler handler_;

  agents::CPU cpu_agent_;
  agents::Memory memory_agent_;
  agents::Network network_agent_;

  SystemConfig config_;
};

} // namespace monsys


#endif // MONITORINGSYSTEM_SRC_MODEL_MODEL_H_