#ifndef MONITORINGSYSTEM_SRC_AGENTS_HANDLER_H_
#define MONITORINGSYSTEM_SRC_AGENTS_HANDLER_H_

#include "base/types.h"

namespace monsys::agents {

class Handler {
 public:
  Handler() noexcept;
  ~Handler();

  AgentStatus ActivateCpuAgent() noexcept;
  AgentStatus ActivateMemoryAgent() noexcept;
  AgentStatus ActivateNetworkAgent() noexcept;

  AgentStatus DeactivateCpuAgent() noexcept;
  AgentStatus DeactivateMemoryAgent() noexcept;
  AgentStatus DeactivateNetworkAgent() noexcept;

 private:
  friend class Builder;

  void *handle_cpu_;
  void *handle_memory_;
  void *handle_network_;
};

} // namespace monsys::agents

#endif // MONITORINGSYSTEM_SRC_AGENTS_HANDLER_H_