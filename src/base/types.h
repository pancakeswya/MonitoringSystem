#ifndef MONITORINGSYSTEM_SRC_BASE_TYPES_H_
#define MONITORINGSYSTEM_SRC_BASE_TYPES_H_

#include <cstddef>
#include <string>
#include <utility>

namespace monsys {

struct Metrics {
  double cpu_load;
  double ram_total;
  double ram;
  double hard_volume;
  double hard_throughput;
  double inet_throughput;
  size_t hard_ops;
  size_t cpu_processes;
  int url_available;
};

using MaxRange = std::pair<double, double>;

struct MetricConfig {
  MaxRange range;
  unsigned int timeout;
};

struct UrlMetricConfig : public MetricConfig {
  std::string url;
};

struct CpuConfig {
  MetricConfig load;
  MetricConfig processes;
};

struct MemoryConfig {
  MetricConfig ram;
  MetricConfig ram_total;
  MetricConfig hard_volume;
  MetricConfig hard_ops;
  MetricConfig hard_throughput;
};

struct NetworkConfig {
  UrlMetricConfig url_available;
  MetricConfig inet_throughout;
};

struct Config {
  CpuConfig cpu;
  MemoryConfig memory;
  NetworkConfig network;
};

}  // namespace monsys

#endif  // MONITORINGSYSTEM_SRC_BASE_TYPES_H_