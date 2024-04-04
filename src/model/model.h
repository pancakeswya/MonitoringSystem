#ifndef MONITORINGSYSTEM_SRC_MODEL_MODEL_H_
#define MONITORINGSYSTEM_SRC_MODEL_MODEL_H_

#include "agents/dll.h"
#include "agents/agents.h"
#include "base/error.h"
#include "base/types.h"

#include <atomic>
#include <string>
#include <unordered_map>
#include <mutex>

namespace monsys {

class Model {
 public:
  using ErrorMap = std::unordered_map<std::string_view, Error>;

  Model() = default;
  ~Model();

  void LogMetrics();
  Error LoadConfig();
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

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_MODEL_MODEL_H_