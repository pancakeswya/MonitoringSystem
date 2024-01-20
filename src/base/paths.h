#ifndef MONITORINGSYSTEM_SRC_BASE_PATHS_H_
#define MONITORINGSYSTEM_SRC_BASE_PATHS_H_

#include <string>

namespace monsys::paths {

#define STRING(x) #x
#define XSTRING(x) STRING(x)

constexpr std::string_view kAgentLib = XSTRING(_CORELIB_PATH);
constexpr std::string_view kAgentCpu = XSTRING(_CORELIB_CPU_PATH);
constexpr std::string_view kAgentMemory = XSTRING(_CORELIB_MEMORY_PATH);
constexpr std::string_view kAgentNetwork = XSTRING(_CORELIB_NETWORK_PATH);
constexpr std::string_view kConfig = XSTRING(_CONFIG_PATH);

inline const std::string kLogs = std::string(XSTRING(_LOG_PATH));
inline const std::string kLogsFile = kLogs + "/metric.logs";

#undef _CORELIB_PATH
#undef _CORELIB_CPU_PATH
#undef _CORELIB_MEMORY_PATH
#undef _CORELIB_NETWORK_PATH
#undef _CONFIG_PATH
#undef _LOG_PATH

#undef XSTRING
#undef STRING

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_BASE_PATHS_H_