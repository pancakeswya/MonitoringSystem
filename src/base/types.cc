#include "base/types.h"
#include <unordered_map>

namespace monsys {

std::string GetStatusString(AgentStatus status) {
  static const std::unordered_map<AgentStatus, std::string> agent_status_map = {
      {AgentStatus::kOk, "Success"},
      {AgentStatus::kNotLoaded, "Agent not loaded"},
      {AgentStatus::kAlreadyActive, "Agent already active"},
      {AgentStatus::kInvalidDeactivate, "Invalid agent deactivation"}
  };
  return agent_status_map.at(status);
}

std::string GetStatusString(MetricStatus status) {
  static const std::unordered_map<MetricStatus, std::string> metric_status_map = {
      {MetricStatus::kOk, "Success"},
      {MetricStatus::kOutOfRange, "Metric is out of range"},
      {MetricStatus::kInvalidUrl, "Invalid url"}
  };
  return metric_status_map.at(status);
}

} // namespace monsys