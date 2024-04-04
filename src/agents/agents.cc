#include "agents/agents.h"
#include "agents/dll.h"

#include <array>
#include <functional>

namespace monsys::agents {

namespace {

template<typename AgentType>
inline std::function<Error()> BuildAgentFn(typename AgentType::type& agent, const Dll& dll) noexcept {
  return [&] () mutable noexcept { auto ret = dll.Get<AgentType>(); agent = ret.first; return ret.second; };
}

} // namespace

std::pair<Cpu, Error> Builder::BuildCpu(const Dll& dll) noexcept {
  Cpu cpu_agent;
  std::array build_calls = {
      BuildAgentFn<CpuLoadFn>(cpu_agent.cpu_load, dll),
      BuildAgentFn<CpuProcessesFn>(cpu_agent.cpu_processes, dll)
  };
  for(const auto& call : build_calls) {
    if (Error err = call(); err) {
      return {{}, err};
    }
  }
  return {cpu_agent, nullerr_t};
}

std::pair<Memory, Error> Builder::BuildMemory(const Dll& dll) noexcept {
  Memory memory_agent;
  std::array build_calls = {
      BuildAgentFn<RamTotalFn>(memory_agent.ram_total, dll),
      BuildAgentFn<RamFn>(memory_agent.ram, dll),
      BuildAgentFn<HardVolumeFn>(memory_agent.hard_volume, dll),
      BuildAgentFn<HardOpsFn>(memory_agent.hard_ops, dll),
      BuildAgentFn<HardThroughputFn>(memory_agent.hard_throughput, dll)
  };
  for(const auto& call : build_calls) {
    if (Error err = call(); err) {
      return {{}, err};
    }
  }
  return {memory_agent, nullerr_t};
}

std::pair<Network, Error> Builder::BuildNetwork(const Dll& dll) noexcept {
  Network network_agent;
  std::array build_calls = {
      BuildAgentFn<UrlAvailableFn>(network_agent.url_available, dll),
      BuildAgentFn<InetThroughputFn>(network_agent.inet_throughput, dll)
  };
  for(const auto& call : build_calls) {
    if (Error err = call(); err) {
      return {{}, err};
    }
  }
  return {network_agent, nullerr_t};
}

} // namespace monsys::agents