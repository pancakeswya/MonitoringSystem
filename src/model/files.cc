#include "model/files.h"

#include <filesystem>
#include <fstream>

namespace monsys::files {

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

inline SystemConfig DefaultConfig() noexcept {
  return SystemConfig {
      {"logger", {}},
      {"cpu", {}},
      {"processes", {}},
      {"ram_total", {}},
      {"ram", {}},
      {"hard_volume", {}},
      {"hard_ops", {}},
      {"hard_throughput", {}},
      {"inet_throughput", {}},
      {"url",
       {.type = "network_agent:ya.ru"}
      }
  };
}

} // namespace

void CreateDirectory(const std::string& path) {
  if (!std::filesystem::exists(path)) {
    std::filesystem::create_directory(path);
  }
}

std::vector<std::string> GetFileList(const std::string& path) {
  std::vector<std::string> file_list;
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    file_list.push_back(entry.path().filename());
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
      last_name = line.substr(i, line.size() - i - 1);
      continue;
    }
    if (last_name.empty()) {
      continue;
    }
    if (field_name == "type") {
      config[last_name].type = line.substr(i + 1, line.size() - i - 1);
    } else if (field_name == "range") {
      if (line[i] == '>') {
        config[last_name].range.first = std::strtod(&line[i + 1], nullptr);
      } else if (line[i] == '<') {
        config[last_name].range.second = std::strtod(&line[i + 1], nullptr);
      } else if (line[i] == '=' && line[i + 1] == '=') {
        double val = std::strtod(&line[i + 2], nullptr);
        config[last_name].range.first = val - 1;
        config[last_name].range.second = val + 1;
      }
    } else if (field_name == "timeout") {
      config[last_name].timeout = std::strtoul(&line[i], nullptr, 10);
    }

  }

  return {true, config};
}

} // namespace monsys
