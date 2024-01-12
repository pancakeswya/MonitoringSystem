#include "model/model.h"
#include "model/config_parser.h"
#include "base/logger.h"
#include "base/paths.h"

#include <filesystem>

namespace monsys {

namespace {

const std::string kFilePath = std::string(" file path: ") + __FILE__;

constexpr const char kFail[] = " fail at line ", kSuccess[] = " success at line ";

template<typename Tp>
inline std::pair<MetricStatus, Tp> RangeBoundsCheck(Tp val, std::pair<double, double> range) noexcept {
  static_assert(std::is_arithmetic_v<Tp>, "Must be arithmetic type");
  if (val < range.first || val > range.second) {
    return {MetricStatus::kOutOfRange, {}};
  }
  return {MetricStatus::kOk, val};
}

inline std::string GetUrl(const std::string& type) {
  size_t pos = type.find(':');
  if (pos == std::string::npos) {
    return "";
  }
  return type.substr(pos + 1);
}

inline const char* GetMetricStatusString(MetricStatus status) {
  return (status == MetricStatus::kOk) ? kSuccess : kFail;
}

} // namespace

Model::Model() noexcept : builder_(&handler_), cpu_agent_(),
                          memory_agent_(), network_agent_() {
  if (!std::filesystem::exists(kLogsPath)) {
    std::filesystem::create_directory(kLogsPath);
  }
}

AgentStatus Model::SetConfig(const std::string& config_path) {
  static const std::string function = __FUNCTION__;
  auto[ok, conf] = ConfigParser::ParseFromFile(config_path);
  if (!ok) {
    Logger::LogAll(function + kFail + std::to_string(__LINE__) + kFilePath,
                   kCpuLogPath, kMemoryLogPath, kNetworkPath);
    return AgentStatus::kNotLoaded;
  }
  config_ = std::move(conf);
  Logger::LogAll(function + kSuccess + std::to_string(__LINE__) + kFilePath,
                 kCpuLogPath, kMemoryLogPath, kNetworkPath);
  return AgentStatus::kOk;
}

AgentStatus Model::LoadCpuAgent() noexcept {
  executed_agent_name_ = "cpu_agent";
  if (auto stat = handler_.ActivateCpuAgent(); stat != AgentStatus::kOk) {
    Logger::Log(kCpuLogPath) << __FUNCTION__ << kFail << __LINE__ << kFilePath << Logger::endlog;
    return stat;
  }
  cpu_agent_ = builder_.BuildCpuAgent();
  Logger::Log(kCpuLogPath) << __FUNCTION__ << kSuccess << __LINE__ << kFilePath << Logger::endlog;
  return AgentStatus::kOk;
}

AgentStatus Model::LoadMemoryAgent() noexcept {
  executed_agent_name_ = "memory_agent";
  if (auto stat = handler_.ActivateMemoryAgent(); stat != AgentStatus::kOk) {
    Logger::Log(kMemoryLogPath) << __FUNCTION__ << kFail << __LINE__ << kFilePath << Logger::endlog;
    return stat;
  }
  Logger::Log(kMemoryLogPath) << __FUNCTION__ << kSuccess << __LINE__ << kFilePath << Logger::endlog;
  memory_agent_ = builder_.BuildMemoryAgent();
  return AgentStatus::kOk;
}

AgentStatus Model::LoadNetworkAgent() noexcept {
  executed_agent_name_ = "network_agent";
  if (auto stat = handler_.ActivateNetworkAgent(); stat != AgentStatus::kOk) {
    Logger::Log(kNetworkPath) << __FUNCTION__ << kFail << __LINE__ << kFilePath << Logger::endlog;
    return stat;
  }
  network_agent_ = builder_.BuildNetworkAgent();
  Logger::Log(kNetworkPath) << __FUNCTION__ << kSuccess << __LINE__ << kFilePath << Logger::endlog;
  return AgentStatus::kOk;
}

AgentStatus Model::UnloadCpuAgent() noexcept {
  executed_agent_name_ = "cpu_agent";
  if (auto stat = handler_.DeactivateCpuAgent(); stat != AgentStatus::kOk) {
    Logger::Log(kCpuLogPath) << __FUNCTION__ << kFail << __LINE__ << kFilePath << Logger::endlog;
    return stat;
  }
  Logger::Log(kCpuLogPath) << __FUNCTION__ << kSuccess << __LINE__ << kFilePath << Logger::endlog;
  return AgentStatus::kOk;
}

AgentStatus Model::UnloadMemoryAgent() noexcept {
  executed_agent_name_ = "memory_agent";
  if (auto stat = handler_.DeactivateMemoryAgent(); stat != AgentStatus::kOk) {
    Logger::Log(kMemoryLogPath) << __FUNCTION__ << kFail << __LINE__ << kFilePath << Logger::endlog;
    return stat;
  }
  Logger::Log(kMemoryLogPath) << __FUNCTION__ << kSuccess << __LINE__ << kFilePath << Logger::endlog;
  return AgentStatus::kOk;
}

AgentStatus Model::UnloadNetworkAgent() noexcept {
  executed_agent_name_ = "network_agent";
  if (auto stat = handler_.DeactivateNetworkAgent(); stat != AgentStatus::kOk) {
    Logger::Log(kNetworkPath) << __FUNCTION__ << kFail << __LINE__ << kFilePath << Logger::endlog;
    return stat;
  }
  Logger::Log(kNetworkPath) << __FUNCTION__ << kSuccess << __LINE__ << kFilePath << Logger::endlog;
  return AgentStatus::kOk;
}

template<typename Callback>
inline auto Model::ExecuteAgent(Callback callback) {
  MetricConfig metric = config_[executed_agent_type_];
  auto val = callback(metric.timeout);
  return RangeBoundsCheck(val, metric.range);
}

std::pair<MetricStatus, double> Model::CpuLoad() {
  executed_agent_name_ = "cpu_agent";
  executed_agent_type_ = "cpu_load";
  auto res = ExecuteAgent(cpu_agent_.cpu_load);
  Logger::Log(kCpuLogPath) << __FUNCTION__ << GetMetricStatusString(res.first) << __LINE__ << kFilePath << Logger::endlog;
  return res;
}

std::pair<MetricStatus, size_t> Model::CpuProcesses() {
  executed_agent_name_ = "cpu_agent";
  executed_agent_type_ = "processes";
  auto res = ExecuteAgent(cpu_agent_.cpu_process);
  Logger::Log(kCpuLogPath) << __FUNCTION__ << GetMetricStatusString(res.first) << __LINE__ << kFilePath << Logger::endlog;
  return res;
}

std::pair<MetricStatus, double> Model::RamTotal() {
  executed_agent_name_ = "memory_agent";
  executed_agent_type_ = "ram_total";
  auto res = ExecuteAgent(memory_agent_.ram_total);
  Logger::Log(kMemoryLogPath) << __FUNCTION__ << GetMetricStatusString(res.first) << __LINE__ << kFilePath << Logger::endlog;
  return res;
}

std::pair<MetricStatus, double> Model::Ram() {
  executed_agent_name_ = "memory_agent";
  executed_agent_type_ = "ram";
  auto res = ExecuteAgent(memory_agent_.ram);
  Logger::Log(kMemoryLogPath) << __FUNCTION__ << GetMetricStatusString(res.first) << __LINE__ << kFilePath << Logger::endlog;
  return res;
}

std::pair<MetricStatus, double> Model::HardVolume() {
  executed_agent_name_ = "memory_agent";
  executed_agent_type_ = "hard_volume";
  auto res = ExecuteAgent(memory_agent_.hard_volume);
  Logger::Log(kMemoryLogPath) << __FUNCTION__ << GetMetricStatusString(res.first) << __LINE__ << kFilePath << Logger::endlog;
  return res;
}

std::pair<MetricStatus, size_t> Model::HardOps() {
  executed_agent_name_ = "memory_agent";
  executed_agent_type_ = "hard_ops";
  auto res = ExecuteAgent(memory_agent_.hard_ops);
  Logger::Log(kMemoryLogPath) << __FUNCTION__ << GetMetricStatusString(res.first) << __LINE__ << kFilePath << Logger::endlog;
  return res;
}

std::pair<MetricStatus, double> Model::HardThroughput() {
  executed_agent_name_ = "memory_agent";
  executed_agent_type_ = "hard_throughput";
  auto res = ExecuteAgent(memory_agent_.hard_throughput);
  Logger::Log(kMemoryLogPath) << __FUNCTION__ << GetMetricStatusString(res.first) << __LINE__ << kFilePath << Logger::endlog;
  return res;
}

std::pair<MetricStatus, int> Model::UrlAvailable() {
  executed_agent_name_ = "network_agent";
  executed_agent_type_ = "url";
  std::string url = GetUrl(config_[executed_agent_type_].type);
  auto res = ExecuteAgent(std::bind(network_agent_.url_available, url.c_str(), std::placeholders::_1));
  Logger::Log(kCpuLogPath) << __FUNCTION__ << GetMetricStatusString(res.first) << __LINE__ << kFilePath << Logger::endlog;
  return res;
}

std::pair<MetricStatus, double> Model::InetThroughput() {
  executed_agent_name_ = "network_agent";
  executed_agent_type_ = "inet_throughput";
  auto res = ExecuteAgent(network_agent_.inet_throughput);
  Logger::Log(kCpuLogPath) << __FUNCTION__ << GetMetricStatusString(res.first) << __LINE__ << kFilePath << Logger::endlog;
  return res;
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
