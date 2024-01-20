#ifndef MONITORINGSYSTEM_SRC_AGENTS_HANDLER_H_
#define MONITORINGSYSTEM_SRC_AGENTS_HANDLER_H_

#include "base/types.h"
#include "base/paths.h"
#include "agents/agents.h"

namespace monsys::agents {

class Handler {
 public:
  Handler() noexcept;
  ~Handler();

  template<typename Tp>
  AgentStatus ActivateAgent() noexcept;

  template<typename Tp>
  void DeactivateAgent() noexcept;

  template<typename Tp>
  bool AgentIsActive() noexcept;

 private:
  friend class Builder;

  static AgentStatus ActivateAgent(void* &handle, const char* path) noexcept;
  static void DeactivateAgent(void* &handle) noexcept;

  void *handle_cpu_;
  void *handle_memory_;
  void *handle_network_;
};

template<>
inline AgentStatus Handler::ActivateAgent<CPU>() noexcept {
  return ActivateAgent(handle_cpu_, paths::kAgentCpu.data());
}

template<>
inline AgentStatus Handler::ActivateAgent<Memory>() noexcept {
  return ActivateAgent(handle_memory_, paths::kAgentMemory.data());
}

template<>
inline AgentStatus Handler::ActivateAgent<Network>() noexcept {
  return ActivateAgent(handle_network_, paths::kAgentNetwork.data());
}

template<>
inline void Handler::DeactivateAgent<CPU>() noexcept {
  DeactivateAgent(handle_cpu_);
}

template<>
inline void Handler::DeactivateAgent<Memory>() noexcept {
  DeactivateAgent(handle_memory_);
}

template<>
inline void Handler::DeactivateAgent<Network>() noexcept {
  DeactivateAgent(handle_network_);
}

template<>
inline bool Handler::AgentIsActive<CPU>() noexcept {
  return handle_cpu_ != nullptr;
}

template<>
inline bool Handler::AgentIsActive<Memory>() noexcept {
  return handle_memory_ != nullptr;
}

template<>
inline bool Handler::AgentIsActive<Network>() noexcept {
  return handle_network_ != nullptr;
}


} // namespace monsys::agents

#endif // MONITORINGSYSTEM_SRC_AGENTS_HANDLER_H_