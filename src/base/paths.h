#ifndef MONITORINGSYSTEM_SRC_BASE_PATHS_H_
#define MONITORINGSYSTEM_SRC_BASE_PATHS_H_

#include <string>

#define STRING(x) #x
#define XSTRING(x) STRING(x)

namespace monsys {

constexpr std::string_view kAgentLibPath = XSTRING(_CORELIB_PATH);
constexpr std::string_view kAgentCpuPath = XSTRING(_CORELIB_CPU_PATH);
constexpr std::string_view kAgentMemoryPath = XSTRING(_CORELIB_MEMORY_PATH);
constexpr std::string_view kAgentNetworkPath = XSTRING(_CORELIB_NETWORK_PATH);

inline const std::string kLogsPath = std::string(XSTRING(_LOG_PATH));
inline const std::string kLogsFilePath = kLogsPath + "/metric.logs";

} // namespace monsys

#undef XSTRING
#undef STRING

#endif // MONITORINGSYSTEM_SRC_BASE_PATHS_H_