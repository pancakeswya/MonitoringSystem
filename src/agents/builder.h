#ifndef MONITORINGSYSTEM_SRC_AGENTS_BUILDER_H_
#define MONITORINGSYSTEM_SRC_AGENTS_BUILDER_H_

#include <agents/agents.h>

namespace monsys::agents {

class Handler;

class Builder {
 public:
  explicit Builder(Handler* handler) noexcept;
  CPU BuildCpuAgent() noexcept;
  Memory BuildMemoryAgent() noexcept;
  Network BuildNetworkAgent() noexcept;

 private:
  Handler* handler_;
};

} // namespace monsys::agents


#endif // MONITORINGSYSTEM_SRC_AGENTS_BUILDER_H_