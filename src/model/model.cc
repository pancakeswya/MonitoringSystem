#include "model/model.h"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <thread>
#include <utility>
#include <vector>

#include "base/config.h"
#include "logger/logger.h"

namespace monsys {

namespace {

constexpr std::string_view kLogPathEnv = "LOG_PATH";
constexpr std::string_view kAgentsPathEnv = "AGENTS_PATH";
constexpr std::string_view kConfigPathEnv = "CONFIG_PATH";

template <typename Tp>
inline Error BuildAgent(Tp& agent, Dll& dll,
                        const std::string& dll_path) noexcept {
  if (Error err = dll.Load(dll_path); err) {
    return err;
  }
  auto [tmp_agent, err] = agents::Builder::Build<Tp>(dll);
  if (err) {
    return err;
  }
  agent = tmp_agent;
  return nullerr_t;
}

template <typename Tp, typename... Args>
inline std::pair<Tp, Error> CollectMetric(CoreError (*metric_fn)(Tp*, Args...),
                                          Args... args) noexcept {
  Tp metric;
  if (CoreError err = metric_fn(&metric, args...); err != kCoreErrorSuccess) {
    return {{}, Error(kCoreErrorsStr[err])};
  }
  return {metric, nullerr_t};
}

template <typename Tp, typename... Args>
inline std::function<void()> MakeMetricCallback(
    Tp& metric, Error& error, MaxRange range, unsigned int timeout,
    CoreError (*metric_fn)(Tp*, Args...), Args... args) noexcept {
  return
      [&metric, &error, range, timeout, metric_fn, args...]() mutable noexcept {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
        auto [tmp_metric, err] =
            CollectMetric(metric_fn, std::forward<Args>(args)...);
        if (err) {
          error = err;
          return;
        }
        if (tmp_metric < range.first || tmp_metric > range.second) {
          error = Error("Metric out of range");
        }
        metric = tmp_metric;
      };
}

}  // namespace

void Model::LogMetrics() {
  const char* kLogPath = std::getenv(kLogPathEnv.data());
  if (!kLogPath || !std::filesystem::exists(kLogPath)) {
    return;
  }
  std::unique_lock<std::mutex> lock(mutex_);
  Logger::Log(kLogPath) << "cpu_load" << metrics_.cpu_load << "cpu_processes"
                        << metrics_.cpu_processes << "ram" << metrics_.ram
                        << "ram_total" << metrics_.ram_total << "hard_ops"
                        << metrics_.hard_ops << "hard_volume"
                        << metrics_.hard_volume << "hard_throughput"
                        << metrics_.hard_throughput << "url_available"
                        << metrics_.url_available << "inet_throughput"
                        << metrics_.inet_throughput << Logger::endlog;
}

Model::ErrorMap Model::LoadAgents() {
  Reset();
  const char* kAgentsPath = std::getenv(kAgentsPathEnv.data());
  if (!kAgentsPath) {
    return {{"LoadAgents", Error("No agents lib path specified")}};
  }
  if (!std::filesystem::exists(kAgentsPath)) {
    error_state_ = true;
    return {{"LoadAgents", Error("Invalid path")}};
  }
  std::unique_lock<std::mutex> lock(mutex_);
  return {{agents::names::kCpu,
           BuildCpuAgent(kAgentsPath + agents::names::kCpuDll)},
          {agents::names::kMemory,
           BuildMemoryAgent(kAgentsPath + agents::names::kMemoryDll)},
          {agents::names::kNetwork,
           BuildNetworkAgent(kAgentsPath + agents::names::kNetworkDll)}};
}

Error Model::LoadConfig() {
  const char* kConfigPath = std::getenv(kConfigPathEnv.data());
  if (!kConfigPath) {
    return Error("No config path specified");
  }
  auto [loaded, config] = config::Read(kConfigPath);
  std::unique_lock<std::mutex> lock(mutex_);
  if (!loaded) {
    error_state_ = true;
    return Error("Config not loaded");
  }
  config_ = config;
  return nullerr_t;
}

Error Model::SaveConfig() {
  const char* kConfigPath = std::getenv(kConfigPathEnv.data());
  if (!kConfigPath) {
    return Error("No config path specified");
  }
  std::unique_lock<std::mutex> lock(mutex_);
  if (!config::Write(config_, kConfigPath)) {
    error_state_ = true;
    return Error("Config not saved");
  }
  return nullerr_t;
}

Config Model::GetConfig() {
  std::unique_lock<std::mutex> lock(mutex_);
  return config_;
}

inline Error Model::BuildCpuAgent(const std::string& dll_path) noexcept {
  return BuildAgent(cpu_agent_, cpu_dll_, dll_path);
}

inline Error Model::BuildMemoryAgent(const std::string& dll_path) noexcept {
  return BuildAgent(memory_agent_, memory_dll_, dll_path);
}

inline Error Model::BuildNetworkAgent(const std::string& dll_path) noexcept {
  return BuildAgent(network_agent_, network_dll_, dll_path);
}

Model::ErrorMap Model::UpdateMetrics() {
  std::unique_lock<std::mutex> lock(mutex_);
  ErrorMap errors = {{agents::CpuLoadFn::kName, nullerr_t},
                     {agents::CpuProcessesFn::kName, nullerr_t},
                     {agents::RamTotalFn::kName, nullerr_t},
                     {agents::RamFn::kName, nullerr_t},
                     {agents::HardOpsFn::kName, nullerr_t},
                     {agents::HardVolumeFn::kName, nullerr_t},
                     {agents::HardThroughputFn::kName, nullerr_t},
                     {agents::UrlAvailableFn::kName, nullerr_t},
                     {agents::InetThroughputFn::kName, nullerr_t}};
  if (error_state_) {
    return errors;
  }
  std::vector calls = {
      /*----------------|--------METRIC_REF---------|-------------------ERROR_REF-------------------|-------------MAX_ACCEPTED_RANGE-----------|-----------METRIC_COLLECT_TIMEOUT----------|--------METRIC_COLLECT_FN----------|---------OPTIONAL_PARAMS_IN_COLLECT_FN-------|*/
      MakeMetricCallback(
          metrics_.cpu_load, /*-------|*/ errors.at(agents::CpuLoadFn::kName),
          /*-------|*/ config_.cpu.load.range, /*---------------|*/ 1,
          /*-------------------------------------|*/ cpu_agent_.cpu_load,
          /*-----------|*/ config_.cpu.load.timeout /*-----------------|*/),
      MakeMetricCallback(
          metrics_.cpu_processes,
          /*--|*/ errors.at(agents::CpuProcessesFn::kName),
          /*--|*/ config_.cpu.processes.range,
          /*----------|*/ config_.cpu.processes.timeout,
          /*---------|*/
          cpu_agent_
              .cpu_processes /*-------|---------------------------------------------|*/),
      MakeMetricCallback(
          metrics_.ram_total, /*------|*/ errors.at(agents::RamTotalFn::kName),
          /*------|*/ config_.memory.ram_total.range,
          /*-------|*/ config_.memory.ram_total.timeout,
          /*------|*/
          memory_agent_
              .ram_total /*--------|---------------------------------------------|*/),
      MakeMetricCallback(
          metrics_.ram, /*------------|*/ errors.at(agents::RamFn::kName),
          /*-----------|*/ config_.memory.ram.range,
          /*-------------|*/ config_.memory.ram.timeout,
          /*------------|*/
          memory_agent_
              .ram /*--------------|---------------------------------------------|*/),
      MakeMetricCallback(
          metrics_.hard_ops, /*-------|*/ errors.at(agents::HardOpsFn::kName),
          /*-------|*/ config_.memory.hard_ops.range,
          /*--------|*/ config_.memory.hard_ops.timeout,
          /*-------|*/
          memory_agent_
              .hard_ops /*---------|---------------------------------------------|*/),
      MakeMetricCallback(
          metrics_.hard_volume,
          /*----|*/ errors.at(agents::HardVolumeFn::kName),
          /*----|*/ config_.memory.hard_volume.range,
          /*-----|*/ config_.memory.hard_volume.timeout,
          /*----|*/
          memory_agent_
              .hard_volume /*------|---------------------------------------------|*/),
      MakeMetricCallback(
          metrics_.hard_throughput,
          /*|*/ errors.at(agents::HardThroughputFn::kName),
          /*|*/ config_.memory.hard_throughput.range,
          /*-|*/ config_.memory.hard_throughput.timeout,
          /*|*/
          memory_agent_
              .hard_throughput /*--|---------------------------------------------|*/),
      MakeMetricCallback(
          metrics_.url_available,
          /*--|*/ errors.at(agents::UrlAvailableFn::kName),
          /*--|*/ config_.network.url_available.range,
          /*--|*/ config_.network.url_available.timeout,
          /*-|*/ network_agent_.url_available,
          /*--|*/ config_.network.url_available.url.c_str() /*|*/),
      MakeMetricCallback(
          metrics_.inet_throughput,
          /*|*/ errors.at(agents::InetThroughputFn::kName),
          /*|*/ config_.network.inet_throughout.range, /*|*/ 1,
          /*-------------------------------------|*/
          network_agent_.inet_throughput,
          /*|*/ config_.network.inet_throughout.timeout /*--|*/)};
  std::vector<std::thread> threads(calls.begin(), calls.end());
  for (std::thread& thread : threads) {
    thread.join();
  }
  return errors;
}

void Model::SetConfig(const Config& config) {
  std::unique_lock<std::mutex> lock(mutex_);
  config_ = config;
}

Metrics Model::GetMetrics() {
  std::unique_lock<std::mutex> lock(mutex_);
  return metrics_;
}

}  // namespace monsys
