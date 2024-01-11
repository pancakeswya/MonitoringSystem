#include "model/model.h"
#include "model/config_parser.h"

#include <type_traits>

namespace monsys {

namespace {

template<typename Tp>
inline std::pair<MetricStatus, Tp> RangeBoundsCheck(Tp val, std::pair<double, double> range) noexcept {
  static_assert(std::is_arithmetic_v<Tp>, "Must be arithmetic type");
  if (val < range.first || val > range.second) {
    return {MetricStatus::kOutOfRange, 0};
  }
  return {MetricStatus::kOk, val};
}

std::string GetUrl(const std::string& type) {
  size_t pos = type.find(':');
  if (pos == std::string::npos) {
    return "";
  }
  return type.substr(pos + 1);
}

} // namespace

Model::Model() noexcept : builder_(&handler_), cpu_agent_(),
                          memory_agent_(), network_agent_() {}

AgentStatus Model::SetConfig(const std::string& config_path) {
  auto[ok, conf] = ConfigParser::ParseFromFile(config_path);
  if (!ok) {
    return AgentStatus::kNotLoaded;
  }
  config_ = std::move(conf);
  return AgentStatus::kOk;
}

AgentStatus Model::LoadCpuAgent() noexcept {
  executed_agent_name_ = "cpu_agent";
  AgentStatus stat = handler_.ActivateCpuAgent();
  if (stat == AgentStatus::kOk) {
    cpu_agent_ = builder_.BuildCpuAgent();
  }
  return stat;
}

AgentStatus Model::LoadMemoryAgent() noexcept {
  executed_agent_name_ = "memory_agent";
  AgentStatus stat = handler_.ActivateMemoryAgent();
  if (stat == AgentStatus::kOk) {
    memory_agent_ = builder_.BuildMemoryAgent();
  }
  return stat;
}

AgentStatus Model::LoadNetworkAgent() noexcept {
  executed_agent_name_ = "network_agent";
  AgentStatus stat = handler_.ActivateNetworkAgent();
  if (stat == AgentStatus::kOk) {
    network_agent_ = builder_.BuildNetworkAgent();
  }
  return stat;
}

template<typename Tp>
std::pair<MetricStatus, Tp> Model::ExecuteAgent(std::function<Tp(unsigned int)> callback) {
  static_assert(std::is_arithmetic_v<Tp>, "Must be arithmetic type");
  MetricConfig metric = config_[executed_agent_type_];
  Tp val = callback(metric.timeout);
  return RangeBoundsCheck(val, metric.range);
}

std::pair<MetricStatus, double> Model::CpuLoad() {
  executed_agent_name_ = "cpu_agent";
  executed_agent_type_ = "cpu_load";
  return ExecuteAgent<double>(cpu_agent_.cpu_load);
}

std::pair<MetricStatus, size_t> Model::CpuProcesses() {
  executed_agent_name_ = "cpu_agent";
  executed_agent_type_ = "processes";
  return ExecuteAgent<size_t>(cpu_agent_.cpu_process);
}

std::pair<MetricStatus, double> Model::RamTotal() {
  executed_agent_name_ = "memory_agent";
  executed_agent_type_ = "ram_total";
  return ExecuteAgent<double>(memory_agent_.ram_total);
}

std::pair<MetricStatus, double> Model::Ram() {
  executed_agent_name_ = "memory_agent";
  executed_agent_type_ = "ram";
  return ExecuteAgent<double>(memory_agent_.ram);
}

std::pair<MetricStatus, double> Model::HardVolume() {
  executed_agent_name_ = "memory_agent";
  executed_agent_type_ = "hard_volume";
  return ExecuteAgent<double>(memory_agent_.hard_volume);
}

std::pair<MetricStatus, size_t> Model::HardOps() {
  executed_agent_name_ = "memory_agent";
  executed_agent_type_ = "hard_ops";
  return ExecuteAgent<size_t>(memory_agent_.hard_ops);
}

std::pair<MetricStatus, double> Model::HardThroughput() {
  executed_agent_name_ = "memory_agent";
  executed_agent_type_ = "hard_throughput";
  return ExecuteAgent<double>(memory_agent_.hard_throughput);
}

std::pair<MetricStatus, int> Model::UrlAvailable() {
  executed_agent_name_ = "network_agent";
  executed_agent_type_ = "url";
  std::string url = GetUrl(config_[executed_agent_type_].type);
  return ExecuteAgent<int>(std::bind(network_agent_.url_available, url.c_str(), std::placeholders::_1));
}

std::pair<MetricStatus, double> Model::InetThroughput() {
  executed_agent_name_ = "network_agent";
  executed_agent_type_ = "inet_throughput";
  return ExecuteAgent<double>(network_agent_.inet_throughput);
}

const std::string& Model::ExecutedAgentType() noexcept {
  return executed_agent_name_;
}

const std::string& Model::ExecutedAgentName() noexcept {
  return executed_agent_type_;
}

void Model::Reset() noexcept {
  handler_.DeactivateCpuAgent();
  handler_.DeactivateMemoryAgent();
  handler_.DeactivateNetworkAgent();

  cpu_agent_ = {};
  memory_agent_ = {};
  network_agent_ = {};
}

} // namespace monsys
