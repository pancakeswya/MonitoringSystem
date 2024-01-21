#include "model/model.h"
#include "model/util.h"
#include "base/logger.h"

#include <thread>
#include <vector>

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

inline void Model::HandleAgentResponse(const AgentResponse& response) {
  if (response.status != AgentStatus::kOk) {
    std::string error_str = "Agent name: " + response.name +
                            "\nError: " + util::GetStatusString(response.status);
    exception_callback_(error_str);
    Logger::Log() << error_str << Logger::endlog;
  }
}

inline void Model::HandleMetricsResponse(const MetricResponse &response) {
  if (response.status != MetricStatus::kOk) {
    std::string error_str ="Agent name: " + response.name +
                           "\nAgent type: " + response.type +
                           "\nError: " + util::GetStatusString(response.status);
    exception_callback_(error_str);
    Logger::Log() << error_str << Logger::endlog;
  }
}


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
inline AgentResponse Model::LoadAgent(Tp& agent, const char* name) {
  AgentStatus stat = handler_.ActivateAgent<Tp>();
  if (stat == AgentStatus::kOk) {
    agent = builder_.BuildAgent<Tp>();
  }
  Logger::Log() << "Agent" << name << "loaded with status" << util::GetStatusString(stat) << Logger::endlog;
  return {stat, name};
}

void Model::LogMetrics(size_t delay) {
  for(;;) {
    std::unique_lock<std::mutex> lock(mutex_);
    bool terminate = cv_.wait_for(lock, std::chrono::milliseconds(delay),
                                         [&state = state_] { return state == State::kLoggerTerminate; } );
    if (terminate) {
      state_ = State::kLoggerStopped;
      cv_.notify_all();
      return;
    }

    cv_.wait(lock, [&state = state_] { return state == State::kIoFree; });

    state_ = State::kIoBusy;
    cv_.notify_all();

    Logger::Log() << "cpu_load" << metrics_.cpu_load << "cpu_processes" << metrics_.cpu_processes
                  << "ram" << metrics_.ram << "ram_total" << metrics_.ram_total << "hard_ops" << metrics_.hard_ops
                  << "hard_volume" << metrics_.hard_volume << "hard_throughput" << metrics_.hard_throughput
                  << "url_available" << metrics_.url_available << "inet_throughput" << metrics_.inet_throughput
                  << Logger::endlog;

    state_ = State::kIoFree;
    cv_.notify_all();
  }
}

AgentResponse Model::SetConfig(const std::string& config_path) {
  constexpr const char name[] = "config";
  auto[ok, conf] = util::ParseConfig(config_path);
  if (!ok) {
    return {AgentStatus::kNotLoaded, name} ;
  }
  config_ = std::move(conf);

  std::thread log_thread(&Model::LogMetrics, this, config_["logger"].timeout);
  log_thread.detach();

  return {AgentStatus::kOk, name};
}

Model::Model() noexcept : state_(State::kIoFree),
                          builder_(&handler_) {
  util::CreateDirectory(paths::kLogs);
}

Model::~Model() {
  std::unique_lock<std::mutex> lock(mutex_);
  cv_.wait(lock, [&state = state_] { return state == State::kIoFree; });

  state_ = State::kLoggerTerminate;
  cv_.notify_all();
  cv_.wait(lock, [&state = state_] { return state == State::kLoggerStopped; });

  state_ = State::kLoadTerminate;
  cv_.notify_all();
  cv_.wait(lock, [&state = state_] { return state == State::kLoadStopped; });
}

void Model::SetExceptionCallback(ExceptionCallback callback) noexcept {
  exception_callback_ = std::move(callback);
}

void Model::LoadAgents() {
  {
    AgentResponse response = SetConfig(paths::kConfig.data());
    HandleAgentResponse(response);
  }
  std::vector<AgentResponse> responses = LoadAgents_();
  for(const AgentResponse& response : responses) {
    HandleAgentResponse(response);
  }

  std::thread load_thread(&Model::LoadAgentsWithDelay, this, config_["load"].timeout);
  load_thread.detach();
}

void Model::UpdateMetrics() {
  std::unique_lock<std::mutex> lock(mutex_);
  cv_.wait(lock, [&state = state_] { return state == State::kIoFree; });

  state_ = State::kIoBusy;
  cv_.notify_all();

  std::vector<MetricResponse> responses = UpdateMetrics_();
  for(const MetricResponse& response : responses) {
    HandleMetricsResponse(response);
  }

  state_ = State::kIoFree;
  cv_.notify_all();
}

void Model::UpdateConfig(const SystemConfig& config) {
  std::unique_lock<std::mutex> lock(mutex_);
  cv_.wait(lock, [&state = state_] { return state == State::kIoFree; });

  state_ = State::kIoBusy;
  cv_.notify_all();

  config_ = config;
  if (bool is_written = util::WriteConfig(config, paths::kConfig.data()); !is_written) {
    std::string error_str = "Failed to write config";
    exception_callback_(error_str);
    Logger::Log() << error_str << Logger::endlog;
  }

  state_ = State::kIoFree;
  cv_.notify_all();
}

std::vector<AgentResponse> Model::LoadAgents_() {
  std::vector<AgentResponse> responses;

  handler_.DeactivateAgent<agents::CPU>();
  handler_.DeactivateAgent<agents::Memory>();
  handler_.DeactivateAgent<agents::Network>();

  if (util::FileExists(paths::kAgentCpu.data())) {
    AgentResponse cpu_response = LoadAgent(cpu_agent_, kCpuAgentName);
    responses.push_back(cpu_response);
  }

  if (util::FileExists(paths::kAgentMemory.data())) {
    AgentResponse memory_response = LoadAgent(memory_agent_, kMemoryAgentName);
    responses.push_back(memory_response);
  }

  if (util::FileExists(paths::kAgentNetwork.data())) {
    AgentResponse network_response = LoadAgent(network_agent_, kNetworkAgentName);
    responses.push_back(network_response);
  }

  return responses;
}

void Model::LoadAgentsWithDelay(size_t delay) {
  for(;;) {
    std::unique_lock<std::mutex> lock(mutex_);
    bool terminate = cv_.wait_for(lock, std::chrono::milliseconds(delay),
                                  [&state = state_] { return state == State::kLoadTerminate; });
    if (terminate) {
      state_ = State::kLoadStopped;
      cv_.notify_all();
      return;
    }

    cv_.wait(lock, [&state = state_] { return state == State::kIoFree; });

    state_ = State::kIoBusy;
    cv_.notify_all();

    std::vector<AgentResponse> responses = LoadAgents_();
    for(const AgentResponse& response : responses) {
      HandleAgentResponse(response);
    }

    state_ = State::kIoFree;
    cv_.notify_all();
  }

}

std::vector<MetricResponse> Model::UpdateMetrics_() {
  std::vector<MetricResponse> responses(agents::kAgentsAmount,
                                        MetricResponse{
                                          .status = MetricStatus::kOk,
                                          .name = "",
                                          .type = ""
                                        }
  );
  std::vector<std::thread> threads;

  if (handler_.AgentIsActive<agents::CPU>()) {
    threads.emplace_back([&] {
      auto [val, response] = ExecuteAgent(cpu_agent_.cpu_load, "cpu");
      metrics_.cpu_load = val;
      responses[0] = std::move(response);
    });

    threads.emplace_back([&] {
      auto [val, response] = ExecuteAgent(cpu_agent_.cpu_process, "process");
      metrics_.cpu_processes = val;
      responses[1] = std::move(response);
    });
  }

  if (handler_.AgentIsActive<agents::Memory>()) {
    threads.emplace_back([&] {
      auto [val, response] = ExecuteAgent(memory_agent_.ram_total, "ram_total");
      metrics_.ram_total = val;
      responses[2] = std::move(response);
    });

    threads.emplace_back([&] {
      auto [val, response] = ExecuteAgent(memory_agent_.ram, "ram");
      metrics_.ram = val;
      responses[3] = std::move(response);
    });

    threads.emplace_back([&] {
      auto [val, response] = ExecuteAgent(memory_agent_.hard_ops, "hard_ops");
      metrics_.hard_ops = val;
      responses[4] = std::move(response);
    });

    threads.emplace_back([&] {
      auto [val, response] = ExecuteAgent(memory_agent_.hard_volume, "hard_volume");
      metrics_.hard_volume = val;
      responses[5] = std::move(response);
    });

    threads.emplace_back([&] {
      auto [val, response] = ExecuteAgent(memory_agent_.hard_throughput, "hard_throughput");
      metrics_.hard_throughput = val;
      responses[6] = std::move(response);
    });
  }

  if (handler_.AgentIsActive<agents::Network>()) {
    threads.emplace_back([&] {
      std::string url = GetUrl(config_["url"].type);
      auto url_callback = std::bind(network_agent_.url_available, url.c_str(), std::placeholders::_1);
      auto [val, response] = ExecuteAgent(url_callback, "url");
      metrics_.url_available = val;
      responses[7] = std::move(response);
    });

    threads.emplace_back([&] {
      auto [val, response] = ExecuteAgent(network_agent_.inet_throughput, "inet_throughput");
      metrics_.inet_throughput = val;
      responses[8] = std::move(response);
    });
  }

  for(std::thread& thread : threads) {
    thread.join();
  }

  return responses;
}

Metrics Model::GetMetrics() noexcept {
  std::unique_lock<std::mutex> lock(mutex_);
  cv_.wait(lock, [&state = state_] { return state == State::kIoFree; });

  state_ = State::kIoBusy;
  cv_.notify_all();

  Metrics metrics = metrics_;

  state_ = State::kIoFree;
  cv_.notify_all();

  return metrics;
}

} // namespace monsys