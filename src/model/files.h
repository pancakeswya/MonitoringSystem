#ifndef MONITORINGSYSTEM_SRC_MODEL_FILES_H_
#define MONITORINGSYSTEM_SRC_MODEL_FILES_H_

#include "model/config.h"

#include <vector>
#include <string>
#include <utility>

namespace monsys::files {

extern void CreateDirectory(const std::string& path);
extern std::vector<std::string> GetFileList(const std::string& path);
extern std::pair<bool, SystemConfig> ParseConfig(const std::string& path);

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_MODEL_FILES_H_