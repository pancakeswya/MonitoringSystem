#ifndef MONITORINGSYSTEM_SRC_BASE_TYPES_H_
#define MONITORINGSYSTEM_SRC_BASE_TYPES_H_

#include <string>

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

std::string GetStatusString(AgentStatus status);
std::string GetStatusString(MetricStatus status);

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_BASE_TYPES_H_