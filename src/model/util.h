#ifndef MONITORINGSYSTEM_SRC_MODEL_UTIL_H_
#define MONITORINGSYSTEM_SRC_MODEL_UTIL_H_

#include "base/types.h"
#include "model/config.h"

#include <vector>
#include <string>
#include <utility>

namespace monsys::util {

extern std::string GetStatusString(AgentStatus status);
extern std::string GetStatusString(MetricStatus status);

extern bool FileExists(const std::string& path);
extern void CreateDirectory(const std::string& path);

extern std::vector<std::string> GetFileList(const std::string& path);
extern std::pair<bool, SystemConfig> ParseConfig(const std::string& path);

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_MODEL_UTIL_H_