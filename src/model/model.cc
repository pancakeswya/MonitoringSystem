#include "model/model.h"
#include "model/config_parser.h"

namespace monsys {

namespace {

inline std::pair<MetricStatus, double> RangeBoundsCheck(double val, std::pair<double, double> range) noexcept {
  if (val < range.first || val > range.second) {
    return {MetricStatus::kOutOfRange, 0.0};
  }
  return {MetricStatus::kOk, val};
}

} // namespace

Model::Model() noexcept : builder_(&handler_), cpu_agent_(),
                          memory_agent_(), network_agent_() {}

agents::AgentStatus Model::SetConfig(const std::string& config_path) {
  auto[ok, conf] = ConfigParser::ParseFromFile(config_path);
  if (!ok) {
    return agents::AgentStatus::kNotLoaded;
  }
  config_ = std::move(conf);
  return agents::AgentStatus::kOk;
}

agents::AgentStatus Model::LoadCpuAgent() noexcept {
  agents::AgentStatus stat = handler_.ActivateCpuAgent();
  cpu_agent_ = builder_.BuildCpuAgent();
  return stat;
}

agents::AgentStatus Model::LoadMemoryAgent() noexcept {
  agents::AgentStatus stat = handler_.ActivateMemoryAgent();
  memory_agent_ = builder_.BuildMemoryAgent();
  return stat;
}

agents::AgentStatus Model::LoadNetworkAgent() noexcept {
  agents::AgentStatus stat = handler_.ActivateNetworkAgent();
  network_agent_ = builder_.BuildNetworkAgent();
  return stat;
}

std::pair<MetricStatus, double> Model::CpuLoad() noexcept {
  MetricConfig metric = config_["cpu_load"];
  double val = cpu_agent_.cpu_load(metric.timeout);
  return RangeBoundsCheck(val, metric.range);
}

std::pair<MetricStatus, size_t> Model::CpuProcesses() noexcept {
  MetricConfig metric = config_["processes"];
  size_t val = cpu_agent_.cpu_process(metric.timeout);
  return RangeBoundsCheck(val, metric.range);
}

std::pair<MetricStatus, double> Model::RamTotal() noexcept {
  MetricConfig metric = config_["ram_total"];
  double val = memory_agent_.ram_total(metric.timeout);
  return RangeBoundsCheck(val, metric.range);
}

std::pair<MetricStatus, double> Model::Ram() noexcept {
  MetricConfig metric = config_["ram"];
  double val = memory_agent_.ram(metric.timeout);
  return RangeBoundsCheck(val, metric.range);
}

std::pair<MetricStatus, double> Model::HardVolume() noexcept {
  MetricConfig metric = config_["hard_volume"];
  double val = memory_agent_.hard_volume(metric.timeout);
  return RangeBoundsCheck(val, metric.range);
}

std::pair<MetricStatus, size_t> Model::HardOps() noexcept {
  MetricConfig metric = config_["hard_ops"];
  size_t val = memory_agent_.hard_ops(metric.timeout);
  return RangeBoundsCheck(val, metric.range);
}

std::pair<MetricStatus, double> Model::HardThroughput() noexcept {
  MetricConfig metric = config_["hard_throughput"];
  double val = memory_agent_.hard_throughput(metric.timeout);
  return RangeBoundsCheck(val, metric.range);
}

std::pair<MetricStatus, int> Model::UrlAvailable() noexcept {
  MetricConfig metric = config_["url"];
  std::string type = metric.type;
  size_t pos = type.find(':');
  if (pos == std::string::npos) {
    return {MetricStatus::kInvalidUrl, 0};
  }
  std::string url = type.substr(pos + 1);
  int val = network_agent_.url_available(url.c_str(), metric.timeout);
  return RangeBoundsCheck(val, metric.range);
}

std::pair<MetricStatus, double> Model::InetThroughput() noexcept {
  MetricConfig metric = config_["inet_throughput"];
  double val = network_agent_.inet_throughput(metric.timeout);
  return RangeBoundsCheck(val, metric.range);
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
