#include "agents/handler.h"

#include <dlfcn.h>

namespace monsys::agents {

constexpr int kDlFlag = RTLD_LAZY;

Handler::Handler() noexcept: handle_cpu_(),
                             handle_memory_(),
                             handle_network_() {}

Handler::~Handler() {
  DeactivateAgent<agents::CPU>();
  DeactivateAgent<agents::Memory>();
  DeactivateAgent<agents::Network>();
}

AgentStatus Handler::ActivateAgent(void* &handle, const char* path) noexcept {
  if (handle) {
    return AgentStatus::kAlreadyActive;
  }
  handle = dlopen(path, kDlFlag);
  if (!handle) {
    return AgentStatus::kNotLoaded;
  }
  return AgentStatus::kOk;
}

void Handler::DeactivateAgent(void* &handle) noexcept {
  if (!handle) {
    return;
  }
  dlclose(handle);
  handle = nullptr;
}

} // namespace monsys::agents