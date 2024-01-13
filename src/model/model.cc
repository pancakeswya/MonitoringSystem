#include "model/model.h"
#include "model/config_parser.h"

#include <thread>
#include <vector>
#include <iostream>

namespace monsys {

namespace {

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

} // namespace

template<typename Callback>
inline auto Model::ExecuteAgent(Callback callback, const char* name) {
  MetricConfig metric_config = config_[name];
  auto curr_val = callback(metric_config.timeout);
  auto[stat, res_val] = RangeBoundsCheck(curr_val, metric_config.range);
  return std::pair<decltype(res_val), MetricResponse>{res_val,
                                                      {.status = stat,
                                                          .name = name,
                                                          .type = metric_config.type
                                                      }};
}

template<typename Tp>
AgentStatus Model::LoadAgent(Tp& agent) {
  AgentStatus stat = handler_.ActivateAgent<Tp>();
  if (stat == AgentStatus::kOk) {
    agent = builder_.BuildAgent<Tp>();
  }
  return stat;
}

AgentResponse Model::SetConfig(const std::string& config_path) {
  constexpr const char name[] = "config";
  auto[ok, conf] = ConfigParser::ParseFromFile(config_path);
  if (!ok) {
    return {AgentStatus::kNotLoaded, name} ;
  }
  config_ = std::move(conf);
  return {AgentStatus::kOk, name};
}

Model::Model() noexcept : builder_(&handler_) {}


AgentResponse Model::LoadCpuAgent() noexcept {
  return {LoadAgent(cpu_agent_), kCpuAgentName};
}

AgentResponse Model::LoadMemoryAgent() noexcept {
  return {LoadAgent(memory_agent_), kMemoryAgentName};
}

AgentResponse Model::LoadNetworkAgent() noexcept {
  return {LoadAgent(network_agent_), kNetworkAgentName};
}

AgentResponse Model::UnloadCpuAgent() noexcept {
  return {handler_.DeactivateAgent<agents::CPU>(), kCpuAgentName};
}

AgentResponse Model::UnloadMemoryAgent() noexcept {
  return {handler_.DeactivateAgent<agents::Memory>(), kNetworkAgentName};
}

AgentResponse Model::UnloadNetworkAgent() noexcept {
  return {handler_.DeactivateAgent<agents::Network>(), kNetworkAgentName};
}

std::vector<MetricResponse> Model::UpdateMetrics() {
  std::vector<MetricResponse> responses(agents::kAgentsAmount,
                                        MetricResponse{
                                          .status = MetricStatus::kOk,
                                          .name = "",
                                          .type = ""
                                        }
  );
  std::vector<std::thread> threads;

  if (handler_.AgentIsActive<agents::CPU>()) {
    threads.emplace_back([&]() {
      auto [val, response] = ExecuteAgent(cpu_agent_.cpu_load, "cpu_load");
      cpu_load_ = val;
      responses[0] = std::move(response);
    });

    threads.emplace_back([&]() {
      auto [val, response] = ExecuteAgent(cpu_agent_.cpu_process, "cpu_process");
      cpu_processes_ = val;
      responses[1] = std::move(response);
    });
  }

  if (handler_.AgentIsActive<agents::Memory>()) {
    threads.emplace_back([&]() {
      auto [val, response] = ExecuteAgent(memory_agent_.ram_total, "ram_total");
      ram_total_ = val;
      responses[2] = std::move(response);
    });

    threads.emplace_back([&]() {
      auto [val, response] = ExecuteAgent(memory_agent_.ram, "ram");
      ram_ = val;
      responses[3] = std::move(response);
    });

    threads.emplace_back([&]() {
      auto [val, response] = ExecuteAgent(memory_agent_.hard_ops, "hard_ops");
      hard_ops_ = val;
      responses[4] = std::move(response);
    });

    threads.emplace_back([&]() {
      auto [val, response] = ExecuteAgent(memory_agent_.hard_volume, "hard_volume");
      hard_volume_ = val;
      responses[5] = std::move(response);
    });

    threads.emplace_back([&]() {
      auto [val, response] = ExecuteAgent(memory_agent_.hard_throughput, "hard_throughput");
      hard_throughput_ = val;
      responses[6] = std::move(response);
    });
  }

  if (handler_.AgentIsActive<agents::Network>()) {
    threads.emplace_back([&]() {
      std::string url = GetUrl(config_["url"].type);
      auto url_callback = std::bind(network_agent_.url_available, url.c_str(), std::placeholders::_1);
      auto [val, response] = ExecuteAgent(url_callback, "url");
      url_available_ = val;
      responses[7] = std::move(response);
    });

    threads.emplace_back([&]() {
      auto [val, response] = ExecuteAgent(network_agent_.inet_throughput, "inet_throughput");
      inet_throughput_ = val;
      responses[8] = std::move(response);
    });
  }

  for(std::thread& thread : threads) {
    thread.join();
  }

  return responses;
}

double Model::CpuLoad() noexcept {
  return cpu_load_;
}

size_t Model::CpuProcesses() noexcept {
  return cpu_processes_;
}

double Model::RamTotal() noexcept {
  return ram_total_;
}

double Model::Ram() noexcept {
  return ram_;
}

double Model::HardVolume() noexcept {
  return hard_volume_;
}

size_t Model::HardOps() noexcept {
  return hard_ops_;
}

double Model::HardThroughput() noexcept {
  return hard_throughput_;
}

int Model::UrlAvailable() noexcept {
  return url_available_;
}

double Model::InetThroughput() noexcept {
  return inet_throughput_;
}

void Model::Reset() noexcept {
  handler_.DeactivateAgent<agents::CPU>();
  handler_.DeactivateAgent<agents::Memory>();
  handler_.DeactivateAgent<agents::Network>();

  cpu_agent_ = {};
  memory_agent_ = {};
  network_agent_ = {};
}

} // namespace monsys