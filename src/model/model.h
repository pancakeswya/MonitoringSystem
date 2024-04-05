#ifndef MONITORINGSYSTEM_SRC_MODEL_MODEL_H_
#define MONITORINGSYSTEM_SRC_MODEL_MODEL_H_

#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>

#include "agents/agents.h"
#include "agents/dll.h"
#include "base/error.h"
#include "base/types.h"

namespace monsys {

class Model {
 public:
  using ErrorMap = std::unordered_map<std::string_view, Error>;

  void LogMetrics();
  Error LoadConfig();
  Error SaveConfig();

  ErrorMap LoadAgents();

  void SetConfig(const Config& config);
  Config GetConfig();

  ErrorMap UpdateMetrics();
  Metrics GetMetrics();

  void Reset() noexcept;

 private:
  Error BuildCpuAgent(const std::string& dll_path) noexcept;
  Error BuildMemoryAgent(const std::string& dll_path) noexcept;
  Error BuildNetworkAgent(const std::string& dll_path) noexcept;

  std::mutex mutex_;
  bool error_state_ = false;

  agents::Cpu cpu_agent_ = {};
  agents::Memory memory_agent_ = {};
  agents::Network network_agent_ = {};

  Metrics metrics_ = {};
  Config config_ = {};

  Dll cpu_dll_ = {}, memory_dll_ = {}, network_dll_ = {};
};

inline void Model::Reset() noexcept {
  cpu_dll_ = {};
  memory_dll_ = {};
  network_dll_ = {};
  cpu_agent_ = {};
  memory_agent_ = {};
  network_agent_ = {};
}

}  // namespace monsys

#endif  // MONITORINGSYSTEM_SRC_MODEL_MODEL_H_
