#ifndef MONITORINGSYSTEM_SRC_MODEL_CONFIG_PARSER_H_
#define MONITORINGSYSTEM_SRC_MODEL_CONFIG_PARSER_H_

#include "model/config.h"

#include <string>
#include <utility>

namespace monsys::ConfigParser {

extern std::pair<bool, SystemConfig> ParseFromFile(const std::string& path);

} // namespace monsys::ConfigParser

#endif // MONITORINGSYSTEM_SRC_MODEL_CONFIG_PARSER_H_