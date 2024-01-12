#include "agents/handler.h"
#include "base/paths.h"

#include <dlfcn.h>

namespace monsys::agents {

constexpr int kDlFlag = RTLD_LAZY;

namespace {

inline AgentStatus ActivateAgent(void* &handle, const char* path) noexcept {
  if (handle) {
    return AgentStatus::kAlreadyActive;
  }
  handle = dlopen(path, kDlFlag);
  if (!handle) {
    return AgentStatus::kNotLoaded;
  }
  return AgentStatus::kOk;
}

inline AgentStatus DeactivateAgent(void* &handle) noexcept {
  if (!handle) {
    return AgentStatus::kInvalidDeactivate;
  }
  dlclose(handle);
  handle = nullptr;
  return AgentStatus::kOk;
}

} // namespace

Handler::Handler() noexcept: handle_cpu_(),
                             handle_memory_(),
                             handle_network_() {}

Handler::~Handler() {
  DeactivateCpuAgent();
  DeactivateMemoryAgent();
  DeactivateNetworkAgent();
}

AgentStatus Handler::ActivateCpuAgent() noexcept {
  return ActivateAgent(handle_cpu_, kAgentCpuPath.data());
}

AgentStatus Handler::ActivateMemoryAgent() noexcept {
  return ActivateAgent(handle_memory_, kAgentMemoryPath.data());
}

AgentStatus Handler::ActivateNetworkAgent() noexcept {
  return ActivateAgent(handle_network_, kAgentNetworkPath.data());
}

AgentStatus Handler::DeactivateCpuAgent() noexcept {
  return DeactivateAgent(handle_cpu_);
}

AgentStatus Handler::DeactivateMemoryAgent() noexcept {
  return DeactivateAgent(handle_memory_);
}

AgentStatus Handler::DeactivateNetworkAgent() noexcept {
  return DeactivateAgent(handle_network_);
}

} // namespace monsys::agents