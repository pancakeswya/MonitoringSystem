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

  AgentStatus LoadCpuAgent() noexcept;
  AgentStatus LoadMemoryAgent() noexcept;
  AgentStatus LoadNetworkAgent() noexcept;

  AgentStatus SetConfig(const std::string& config_path);

  std::pair<MetricStatus, double> CpuLoad();
  std::pair<MetricStatus, size_t> CpuProcesses();

  std::pair<MetricStatus, double> RamTotal();
  std::pair<MetricStatus, double> Ram();
  std::pair<MetricStatus, double> HardVolume();
  std::pair<MetricStatus, size_t> HardOps();
  std::pair<MetricStatus, double> HardThroughput();

  std::pair<MetricStatus, double> InetThroughput();
  std::pair<MetricStatus, int> UrlAvailable();

  const std::string& ExecutedAgentType() noexcept;
  const std::string& ExecutedAgentName() noexcept;

  void Reset() noexcept;
 private:
  template<typename Tp>
  std::pair<MetricStatus, Tp> ExecuteAgent(std::function<Tp(unsigned int)> callback);

  agents::Builder builder_;
  agents::Handler handler_;

  agents::CPU cpu_agent_;
  agents::Memory memory_agent_;
  agents::Network network_agent_;

  std::string executed_agent_name_;
  std::string executed_agent_type_;

  SystemConfig config_;
};

} // namespace monsys


#endif // MONITORINGSYSTEM_SRC_MODEL_MODEL_H_