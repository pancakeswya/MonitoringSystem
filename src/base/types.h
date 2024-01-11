#ifndef MONITORINGSYSTEM_SRC_BASE_TYPES_H_
#define MONITORINGSYSTEM_SRC_BASE_TYPES_H_

namespace monsys {

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

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_BASE_TYPES_H_