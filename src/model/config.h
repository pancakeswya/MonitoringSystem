#ifndef MONITORINGSYSTEM_SRC_MODEL_CONFIG_H_
#define MONITORINGSYSTEM_SRC_MODEL_CONFIG_H_

#include <string>
#include <utility>
#include <limits>
#include <unordered_map>

namespace monsys {

struct MetricConfig {
  static constexpr unsigned int default_timeout = 990000;

  std::string type{};
  std::pair<double, double> range{-std::numeric_limits<double>::max(),
                                   std::numeric_limits<double>::max()};
  unsigned int timeout = default_timeout;
};

using SystemConfig = std::unordered_map<std::string, MetricConfig>;

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_MODEL_CONFIG_H_