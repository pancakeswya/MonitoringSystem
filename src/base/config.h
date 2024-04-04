#ifndef MONITORINGSYSTEM_SRC_BASE_CONFIG_H_
#define MONITORINGSYSTEM_SRC_BASE_CONFIG_H_

#include "base/types.h"

namespace monsys::config {

std::pair<bool, Config> Read(const std::string& path);
bool Write(Config& config, const std::string& path);

} // namespace conf

#endif // MONITORINGSYSTEM_SRC_BASE_CONFIG_H_