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

std::string GetStatusString(AgentStatus status);
std::string GetStatusString(MetricStatus status);

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_BASE_TYPES_H_