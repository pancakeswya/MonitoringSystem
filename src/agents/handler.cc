#include "agents/handler.h"
#include "agents/agents.h"

#include <dlfcn.h>

#define __STRING(x) #x
#define __XSTRING(x) __STRING(x)

namespace monsys::agents {

constexpr const char kAgentCpuPath[] = __XSTRING(_CORELIB_CPU_PATH);
constexpr const char kAgentMemoryPath[] = __XSTRING(_CORELIB_MEMORY_PATH);
constexpr const char kAgentNetworkPath[] = __XSTRING(_CORELIB_NETWORK_PATH);

constexpr int kDlFlag = RTLD_LAZY;

Handler::Handler() noexcept: handle_cpu_(),
                             handle_memory_(),
                             handle_network_() {}

Handler::~Handler() {
  DeactivateCpuAgent();
  DeactivateMemoryAgent();
  DeactivateNetworkAgent();
}

AgentStatus Handler::ActivateCpuAgent() noexcept {
  if (handle_cpu_) {
    return AgentStatus::kAlreadyActive;
  }
  handle_cpu_ = dlopen(kAgentCpuPath, kDlFlag);
  if (!handle_cpu_) {
    return AgentStatus::kNotLoaded;
  }
  return AgentStatus::kOk;
}

AgentStatus Handler::ActivateMemoryAgent() noexcept {
  if (handle_memory_) {
    return AgentStatus::kAlreadyActive;
  }
  handle_memory_ = dlopen(kAgentMemoryPath, kDlFlag);
  if (!handle_memory_) {
    return AgentStatus::kNotLoaded;
  }
  return AgentStatus::kOk;
}

AgentStatus Handler::ActivateNetworkAgent() noexcept {
  if (handle_network_) {
    return AgentStatus::kAlreadyActive;
  }
  handle_network_ = dlopen(kAgentNetworkPath, kDlFlag);
  if (!handle_network_) {
    return AgentStatus::kNotLoaded;
  }
  return AgentStatus::kOk;
}

AgentStatus Handler::DeactivateCpuAgent() noexcept {
  if (!handle_cpu_) {
    return AgentStatus::kInvalidDeactivate;
  }
  dlclose(handle_cpu_);
  handle_cpu_ = nullptr;
  return AgentStatus::kOk;
}

AgentStatus Handler::DeactivateMemoryAgent() noexcept {
  if (!handle_memory_) {
    return AgentStatus::kInvalidDeactivate;
  }
  dlclose(handle_memory_);
  handle_memory_ = nullptr;
  return AgentStatus::kOk;
}

AgentStatus Handler::DeactivateNetworkAgent() noexcept {
  if (!handle_network_) {
    return AgentStatus::kInvalidDeactivate;
  }
  dlclose(handle_network_);
  handle_network_ = nullptr;
  return AgentStatus::kOk;
}

} // namespace monsys::agents