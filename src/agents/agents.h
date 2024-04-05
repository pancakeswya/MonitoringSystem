#ifndef MONITORINGSYSTEM_SRC_AGENTS_AGENTS_H_
#define MONITORINGSYSTEM_SRC_AGENTS_AGENTS_H_

#include <cstddef>
#include <utility>

#include "agents/core/include/defs.h"
#include "base/error.h"

namespace monsys {

class Dll;

namespace agents {

namespace names {

constexpr std::string_view kCpu = "cpu_agent";
constexpr std::string_view kMemory = "memory_agent";
constexpr std::string_view kNetwork = "network_agent";

inline std::string kCpuDll = "/libcpu.so";
inline std::string kMemoryDll = "/libmemory.so";
inline std::string kNetworkDll = "/libnetwork.so";

}  // namespace names

constexpr size_t kAmount = 9;

struct CpuLoadFn {
  using type = CoreError (*)(double *, unsigned int) noexcept;
  static constexpr std::string_view kName = "CpuLoad";
};

struct CpuProcessesFn {
  using type = CoreError (*)(size_t *) noexcept;
  static constexpr std::string_view kName = "CpuProcesses";
};

struct RamTotalFn {
  using type = CoreError (*)(double *) noexcept;
  static constexpr std::string_view kName = "RamTotal";
};

struct RamFn {
  using type = CoreError (*)(double *) noexcept;
  static constexpr std::string_view kName = "Ram";
};

struct HardVolumeFn {
  using type = CoreError (*)(double *) noexcept;
  static constexpr std::string_view kName = "HardVolume";
};

struct HardOpsFn {
  using type = CoreError (*)(size_t *) noexcept;
  static constexpr std::string_view kName = "HardOps";
};

struct HardThroughputFn {
  using type = CoreError (*)(double *) noexcept;
  static constexpr std::string_view kName = "HardThroughput";
};

struct UrlAvailableFn {
  using type = CoreError (*)(int *, const char *) noexcept;
  static constexpr std::string_view kName = "UrlAvailable";
};

struct InetThroughputFn {
  using type = CoreError (*)(double *, unsigned int) noexcept;
  static constexpr std::string_view kName = "InetThroughput";
};

struct Cpu {
  CpuLoadFn::type cpu_load;
  CpuProcessesFn::type cpu_processes;
};

struct Memory {
  RamTotalFn::type ram_total;
  RamFn::type ram;
  HardVolumeFn::type hard_volume;
  HardOpsFn::type hard_ops;
  HardThroughputFn::type hard_throughput;
};

struct Network {
  UrlAvailableFn::type url_available;
  InetThroughputFn::type inet_throughput;
};

class Builder {
 public:
  template <typename Tp>
  static std::pair<Tp, Error> Build(const Dll &) noexcept;

 private:
  static std::pair<Cpu, Error> BuildCpu(const Dll &) noexcept;
  static std::pair<Memory, Error> BuildMemory(const Dll &) noexcept;
  static std::pair<Network, Error> BuildNetwork(const Dll &) noexcept;
};

template <>
inline std::pair<Cpu, Error> Builder::Build(const monsys::Dll &dll) noexcept {
  return BuildCpu(dll);
}

template <>
inline std::pair<Memory, Error> Builder::Build(
    const monsys::Dll &dll) noexcept {
  return BuildMemory(dll);
}

template <>
inline std::pair<Network, Error> Builder::Build(
    const monsys::Dll &dll) noexcept {
  return BuildNetwork(dll);
}

}  // namespace agents

}  // namespace monsys

#endif  // MONITORINGSYSTEM_SRC_AGENTS_AGENTS_H_