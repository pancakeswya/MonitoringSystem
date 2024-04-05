#include "base/config.h"

#include <fstream>
#include <string>
#include <unordered_map>

namespace monsys::config {

namespace {

using MetricMap =
    std::unordered_map<std::string,
                       std::unordered_map<std::string, MetricConfig>>;
using MetricRefMap =
    std::unordered_map<std::string,
                       std::unordered_map<std::string, MetricConfig*>>;

size_t SkipSpace(const std::string& str) noexcept {
  size_t i = 0;
  for (; std::isspace(str[i]); ++i)
    ;
  return i;
}

size_t SkipAlpha(const std::string& str) noexcept {
  size_t i = 0;
  for (; std::isalpha(str[i]); ++i)
    ;
  return i;
}

size_t LastAlpha(const std::string& str) noexcept {
  size_t i = str.size() - 1;
  for (; !std::isalpha(str[i]); --i)
    ;
  return i + 1;
}

MetricMap ParseMetricFields(std::ifstream& input, std::string& url) {
  MetricMap metric_map;
  std::string line, last_name, last_type;
  while (std::getline(input, line)) {
    size_t i = SkipAlpha(line);
    std::string field_name = line.substr(0, i++);
    i += SkipSpace(&line[i]);
    if (field_name == "name") {
      last_name = line.substr(i, LastAlpha(line) - i);
      continue;
    }
    if (field_name == "type") {
      last_type = line.substr(i, LastAlpha(line) - i);
      continue;
    }
    if (field_name == "url") {
      url = line.substr(i, LastAlpha(line) - i);
      continue;
    }
    if (last_name.empty() || last_type.empty()) {
      continue;
    }
    if (field_name == "range") {
      auto& [min, max] = metric_map[last_type][last_name].range;
      while (i < line.size()) {
        size_t parsed_size = 1;
        if (line[i] == '>') {
          if (line[i + 1] == '=') {
            i += 2;
            min = std::stod(&line[i], &parsed_size) - 1;
          } else {
            min = std::stod(&line[++i], &parsed_size);
          }
        } else if (line[i] == '<') {
          max = std::stod(&line[++i], &parsed_size);
        } else if (line[i] == '=') {
          if (line[i + 1] == '=') {
            i += 2;
            double val = std::stod(&line[i], &parsed_size);
            min = val - 1;
            max = val + 1;
          } else if (line[i + 1] == '<') {
            i += 2;
            max = std::stod(&line[i], &parsed_size) + 1;
          }
        }
        i += parsed_size;
      }
    } else if (field_name == "timeout") {
      metric_map[last_type][last_name].timeout = std::stoul(&line[i]);
    }
  }
  return metric_map;
}

MetricRefMap MetricRefMapFromConfig(Config& config) {
  return {{"cpu_agent",
           {
               {"cpu", &config.cpu.load},
               {"processes", &config.cpu.processes},
           }},
          {"memory_agent",
           {{"ram", &config.memory.ram},
            {"ram_total", &config.memory.ram_total},
            {"hard_volume", &config.memory.hard_volume},
            {"hard_ops", &config.memory.hard_ops},
            {"hard_throughput", &config.memory.hard_throughput}}},
          {"network_agent",
           {{"url_available", &config.network.url_available},
            {"inet_throughput", &config.network.inet_throughout}}}};
}

std::pair<bool, Config> ConfigFromMetricMap(const MetricMap& metric_map,
                                            const std::string& url) {
  Config config = {};
  MetricRefMap res_map = MetricRefMapFromConfig(config);
  for (auto& [type, named_metric] : res_map) {
    for (auto& [name, metric] : named_metric) {
      try {
        *res_map[type][name] = metric_map.at(type).at(name);
      } catch (...) {
        return {false, {}};
      }
    }
  }
  config.network.url_available.url = url;
  return {true, config};
}

}  // namespace

std::pair<bool, Config> Read(const std::string& path) {
  std::ifstream input(path.data(), std::ifstream::binary);
  if (!input.is_open()) {
    return {false, {}};
  }
  std::string url;
  MetricMap metric_map = ParseMetricFields(input, url);
  return ConfigFromMetricMap(metric_map, url);
}

bool Write(Config& config, const std::string& path) {
  std::ofstream ofs(path.data());
  if (!ofs.is_open()) {
    return false;
  }
  MetricRefMap metric_ref_map = MetricRefMapFromConfig(config);
  for (auto& [type, named_metric] : metric_ref_map) {
    for (auto& [name, metric] : named_metric) {
      ofs << "name: " << name << '\n';
      if (!type.empty()) {
        ofs << "type: " << type << '\n';
      }
      auto [min, max] = metric->range;
      ofs << "range: > " << min << ", < " << max << '\n';
      ofs << "timeout: " << metric->timeout << "\n\n";
    }
  }
  ofs << "url: " << config.network.url_available.url << '\n';
  return true;
}

}  // namespace monsys::config
