#include "model/util.h"

#include <filesystem>
#include <fstream>

#include <iostream>

namespace monsys::util {

namespace {

size_t SkipSpace(const std::string& str) noexcept {
  size_t i = 0;
  for(;std::isspace(str[i]); ++i)
    ;
  return i;
}

size_t SkipAlpha(const std::string& str) noexcept {
  size_t i = 0;
  for(;std::isalpha(str[i]); ++i)
    ;
  return i;
}

} // namespace

std::string GetStatusString(AgentStatus status) {
  static const std::unordered_map<AgentStatus, std::string> agent_status_map = {
      {AgentStatus::kOk, "Success"},
      {AgentStatus::kNotLoaded, "Agent not loaded"},
      {AgentStatus::kAlreadyActive, "Agent already active"},
      {AgentStatus::kInvalidDeactivate, "Invalid agent deactivation"}
  };
  return agent_status_map.at(status);
}

std::string GetStatusString(MetricStatus status) {
  static const std::unordered_map<MetricStatus, std::string> metric_status_map = {
      {MetricStatus::kOk, "Success"},
      {MetricStatus::kOutOfRange, "Metric is out of range"},
      {MetricStatus::kInvalidUrl, "Invalid url"}
  };
  return metric_status_map.at(status);
}

bool FileExists(const std::string& path) {
  return std::filesystem::exists(path);
}

void CreateDirectory(const std::string& path) {
  if (!std::filesystem::exists(path)) {
    std::filesystem::create_directory(path);
  }
}

std::vector<std::string> GetFileList(const std::string& path) {
  std::vector<std::string> file_list;
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    file_list.push_back(entry.path().string());
  }
  return file_list;
}

std::pair<bool, SystemConfig> ParseConfig(const std::string& path) {
  SystemConfig config = DefaultConfig();
  std::ifstream input(path.data(), std::ifstream::binary);
  if (!input.is_open()) {
    return {false, {}};
  }
  std::string line, last_name;
  while(std::getline(input, line)) {
    size_t i = SkipAlpha(line);
    std::string field_name = line.substr(0, i++);
    i += SkipSpace(&line[i]);
    if (field_name == "name") {
      last_name = line.substr(i, line.size() - i);
      continue;
    }
    if (last_name.empty()) {
      continue;
    }
    if (field_name == "type") {
      config[last_name].type = line.substr(i, line.size() - i);
    } else if (field_name == "range") {
      auto&[min, max] = config[last_name].range;
      while(i < line.size()) {
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
      config[last_name].timeout = std::stoul(&line[i]);
    }
  }
  return {true, config};
}

bool WriteConfig(const SystemConfig& config, const std::string& path) {
  std::ofstream ofs(path.data());
  if (!ofs.is_open()) {
    return false;
  }
  for(auto&[name, value] : config) {
    ofs << "name: " << name << '\n';
    auto&[type, range, timeout] = value;
    auto[min, max] = range;
    if (!type.empty()) {
      ofs << "type: " << type << '\n';
    }
    if (min != 0 && max != 0) {
      ofs << "range: > " << min << ", < " << max << '\n';
    }
    ofs << "timeout: " << timeout << "\n\n";
  }
  return true;
}

} // namespace monsys
