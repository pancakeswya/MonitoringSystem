#ifndef MONITORINGSYSTEM_SRC_AGENTS_BUILDER_H_
#define MONITORINGSYSTEM_SRC_AGENTS_BUILDER_H_

#include <agents/agents.h>

namespace monsys::agents {

class Handler;

class Builder {
 public:
  explicit Builder(Handler* handler) noexcept;

  template<typename Tp>
  Tp BuildAgent() noexcept;

 private:
  CPU BuildCpuAgent() noexcept;
  Memory BuildMemoryAgent() noexcept;
  Network BuildNetworkAgent() noexcept;
  Handler* handler_;
};

template<>
inline CPU Builder::BuildAgent() noexcept {
  return BuildCpuAgent();
}

template<>
inline Memory Builder::BuildAgent() noexcept {
  return BuildMemoryAgent();
}

template<>
inline Network Builder::BuildAgent() noexcept {
  return BuildNetworkAgent();
}

} // namespace monsys::agents


#endif // MONITORINGSYSTEM_SRC_AGENTS_BUILDER_H_