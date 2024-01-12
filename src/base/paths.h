#ifndef MONITORINGSYSTEM_SRC_BASE_PATHS_H_
#define MONITORINGSYSTEM_SRC_BASE_PATHS_H_

#include <string>

#define __STRING(x) #x
#define __XSTRING(x) __STRING(x)

namespace monsys {

constexpr std::string_view kAgentCpuPath = __XSTRING(_CORELIB_CPU_PATH);
constexpr std::string_view kAgentMemoryPath = __XSTRING(_CORELIB_MEMORY_PATH);
constexpr std::string_view kAgentNetworkPath = __XSTRING(_CORELIB_NETWORK_PATH);

inline const std::string kLogsPath = std::string(__XSTRING(_LOG_PATH));
inline const std::string kCpuLogPath = kLogsPath + "/cpu.log";
inline const std::string kMemoryLogPath = kLogsPath + "/memory.log";
inline const std::string kNetworkPath = kLogsPath + "/network.log";

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_BASE_PATHS_H_