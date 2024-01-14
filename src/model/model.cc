#include "model/model.h"
#include "model/config_parser.h"
#include "base/logger.h"

#include <thread>
#include <vector>
#include <filesystem>

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

void Model::LogMetrics(size_t delay) {
  for(;;) {
    std::unique_lock<std::mutex> lock(mutex_);
    bool about_to_destroy = cv_.wait_for(lock, std::chrono::milliseconds(delay),
                                         [&state = state_] { return state == ModelState::kAboutToDestroy; } );
    if (about_to_destroy) {
      state_ = ModelState::kStoppedLogger;
      cv_.notify_all();
      return;
    }

    cv_.wait(lock, [&state = state_] { return state == ModelState::kIoFree; });

    state_ = ModelState::kIoBusy;
    cv_.notify_all();

    Logger::Log() << "cpu_load" << metrics_.cpu_load << "cpu_processes" << metrics_.cpu_processes
                  << "ram" << metrics_.ram << "ram_total" << metrics_.ram_total << "hard_ops" << metrics_.hard_ops
                  << "hard_volume" << metrics_.hard_volume << "hard_throughput" << metrics_.hard_throughput
                  << "url_available" << metrics_.url_available << "inet_throughput" << metrics_.inet_throughput
                  << Logger::endlog;

    state_ = ModelState::kIoFree;
    cv_.notify_all();
  }
}

AgentResponse Model::SetConfig(const std::string& config_path) {
  constexpr const char name[] = "config";
  auto[ok, conf] = ConfigParser::ParseFromFile(config_path);
  if (!ok) {
    return {AgentStatus::kNotLoaded, name} ;
  }
  config_ = std::move(conf);

  std::thread log_thread(&Model::LogMetrics, this, config_["logger"].timeout);
  log_thread.detach();

  return {AgentStatus::kOk, name};
}

Model::Model() noexcept : state_(ModelState::kIoFree),
                          builder_(&handler_) {
  if (!std::filesystem::exists(kLogsPath)) {
    std::filesystem::create_directory(kLogsPath);
  }
}

Model::~Model() {
  std::unique_lock<std::mutex> lock(mutex_);
  cv_.wait(lock, [&state = state_] { return state == ModelState::kIoFree; });
  state_ = ModelState::kAboutToDestroy;
  cv_.notify_all();
  cv_.wait(lock, [&state = state_] { return state == ModelState::kStoppedLogger; });
}

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
  std::unique_lock<std::mutex> lock(mutex_);
  cv_.wait(lock, [&state = state_] { return state == ModelState::kIoFree; });

  state_ = ModelState::kIoBusy;
  cv_.notify_all();

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
      auto [val, response] = ExecuteAgent(cpu_agent_.cpu_load, "cpu");
      metrics_.cpu_load = val;
      responses[0] = std::move(response);
    });

    threads.emplace_back([&]() {
      auto [val, response] = ExecuteAgent(cpu_agent_.cpu_process, "process");
      metrics_.cpu_processes = val;
      responses[1] = std::move(response);
    });
  }

  if (handler_.AgentIsActive<agents::Memory>()) {
    threads.emplace_back([&]() {
      auto [val, response] = ExecuteAgent(memory_agent_.ram_total, "ram_total");
      metrics_.ram_total = val;
      responses[2] = std::move(response);
    });

    threads.emplace_back([&]() {
      auto [val, response] = ExecuteAgent(memory_agent_.ram, "ram");
      metrics_.ram = val;
      responses[3] = std::move(response);
    });

    threads.emplace_back([&]() {
      auto [val, response] = ExecuteAgent(memory_agent_.hard_ops, "hard_ops");
      metrics_.hard_ops = val;
      responses[4] = std::move(response);
    });

    threads.emplace_back([&]() {
      auto [val, response] = ExecuteAgent(memory_agent_.hard_volume, "hard_volume");
      metrics_.hard_volume = val;
      responses[5] = std::move(response);
    });

    threads.emplace_back([&]() {
      auto [val, response] = ExecuteAgent(memory_agent_.hard_throughput, "hard_throughput");
      metrics_.hard_throughput = val;
      responses[6] = std::move(response);
    });
  }

  if (handler_.AgentIsActive<agents::Network>()) {
    threads.emplace_back([&]() {
      std::string url = GetUrl(config_["url"].type);
      auto url_callback = std::bind(network_agent_.url_available, url.c_str(), std::placeholders::_1);
      auto [val, response] = ExecuteAgent(url_callback, "url");
      metrics_.url_available = val;
      responses[7] = std::move(response);
    });

    threads.emplace_back([&]() {
      auto [val, response] = ExecuteAgent(network_agent_.inet_throughput, "inet_throughput");
      metrics_.inet_throughput = val;
      responses[8] = std::move(response);
    });
  }

  for(std::thread& thread : threads) {
    thread.join();
  }

  state_ = ModelState::kIoFree;
  cv_.notify_all();

  return responses;
}

Metrics Model::GetMetrics() noexcept {
  std::unique_lock<std::mutex> lock(mutex_);
  cv_.wait(lock, [&state = state_] { return state == ModelState::kIoFree; });

  state_ = ModelState::kIoBusy;
  cv_.notify_all();

  Metrics metrics = metrics_;

  state_ = ModelState::kIoFree;
  cv_.notify_all();

  return metrics;
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