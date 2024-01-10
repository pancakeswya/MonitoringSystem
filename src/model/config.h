#ifndef MONITORINGSYSTEM_SRC_MODEL_CONFIG_H_
#define MONITORINGSYSTEM_SRC_MODEL_CONFIG_H_

#include <string>
#include <utility>
#include <limits>
#include <unordered_map>

namespace monsys {

constexpr unsigned int default_timeout = 990000;

struct MetricConfig {
  std::string type{};
  std::pair<double, double> range{-std::numeric_limits<double>::max(),
                                   std::numeric_limits<double>::max()};
  size_t timeout = default_timeout;
};

using SystemConfig = std::unordered_map<std::string, MetricConfig>;

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_MODEL_CONFIG_H_