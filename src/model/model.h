#ifndef MONITORINGSYSTEM_SRC_MODEL_MODEL_H_
#define MONITORINGSYSTEM_SRC_MODEL_MODEL_H_

#include "agents/agents.h"
#include "agents/builder.h"
#include "agents/handler.h"
#include "model/config.h"

#include <string>
#include <utility>

namespace monsys {

enum class MetricStatus : unsigned char {
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

  std::pair<MetricStatus, double> CpuLoad();
  std::pair<MetricStatus, size_t> CpuProcesses();

  std::pair<MetricStatus, double> RamTotal();
  std::pair<MetricStatus, double> Ram();
  std::pair<MetricStatus, double> HardVolume();
  std::pair<MetricStatus, size_t> HardOps();
  std::pair<MetricStatus, double> HardThroughput();

  std::pair<MetricStatus, double> InetThroughput();
  std::pair<MetricStatus, int> UrlAvailable();

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