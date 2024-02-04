#ifndef MONITORINGSYSTEM_SRC_BASE_TYPES_H_
#define MONITORINGSYSTEM_SRC_BASE_TYPES_H_

#include <string>
#include <functional>
#include <limits>

namespace monsys {

constexpr const char kCpuAgentName[] = "cpu_agent",
                     kMemoryAgentName[] = "memory_agent",
                     kNetworkAgentName[] = "network_agent";

enum class AgentStatus : unsigned char {
  kOk,
  kNotLoaded,
  kAlreadyActive,
  kInvalidDeactivate
};

enum class MetricStatus : unsigned char {
  kOk,
  kOutOfRange,
  kInvalidUrl
};

struct AgentResponse {
  AgentStatus status;
  std::string name;
};

struct MetricResponse {
  MetricStatus status;
  std::string name;
  std::string type;
};

struct Metrics {
  double cpu_load;
  size_t cpu_processes;

  double ram_total;
  double ram;
  double hard_volume;
  size_t hard_ops;
  double hard_throughput;

  double inet_throughput;
  int url_available;
};

struct MetricConfig {
  static constexpr unsigned int default_timeout = 990000;

  std::string type{};
  std::pair<double, double> range{0.0, std::numeric_limits<double>::max()};
  unsigned int timeout = default_timeout;
};

using SystemConfig = std::unordered_map<std::string, MetricConfig>;

using ExceptionCallback = std::function<void(const std::string&)>;

namespace {

inline SystemConfig DefaultConfig() noexcept {
  return {
      {"logger", {
        .range = {}
      }},
      {"cpu", {
          .type = "cpu_agent"
      }},
      {"processes", {
          .type = "cpu_agent"
      }},
      {"ram_total", {
          .type = "memory_agent"
      }},
      {"ram", {
          .type = "memory_agent"
      }},
      {"hard_volume", {
          .type = "memory_agent"
      }},
      {"hard_ops", {
          .type = "memory_agent"
      }},
      {"hard_throughput", {
          .type = "memory_agent"
      }},
      {"inet_throughput", {
          .type = "network_agent"
      }},
      {"url",
       {.type = "network_agent:ya.ru"}
      }
  };
}

} // namespace

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_BASE_TYPES_H_
