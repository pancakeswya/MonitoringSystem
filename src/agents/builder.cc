#include "agents/builder.h"
#include "agents/handler.h"
#include "agents/agents.h"

#include <cassert>
#include <dlfcn.h>

namespace monsys::agents {

Builder::Builder(Handler* handler) noexcept : handler_(handler) {}

CPU Builder::BuildCpuAgent() noexcept {
  assert(handler_->handle_cpu_);
  return {
      .cpu_load = reinterpret_cast<CpuLoadCallback>(
          dlsym(handler_->handle_cpu_, "CpuLoad")
      ),
      .cpu_process = reinterpret_cast<CpuProcessCallback>(
          dlsym(handler_->handle_cpu_, "CpuProcesses")
      )
  };
}

Memory Builder::BuildMemoryAgent() noexcept {
  assert(handler_->handle_memory_);
  return {
      .ram_total = reinterpret_cast<RamTotalCallback>(
          dlsym(handler_->handle_memory_, "RamTotal")
      ),
      .ram = reinterpret_cast<RamCallback>(
          dlsym(handler_->handle_memory_, "Ram")
      ),
      .hard_volume = reinterpret_cast<HardVolumeCallback>(
          dlsym(handler_->handle_memory_, "HardVolume")
      ),
      .hard_ops = reinterpret_cast<HardOpsCallback>(
          dlsym(handler_->handle_memory_, "HardOps")
      ),
      .hard_throughput = reinterpret_cast<HardThroughputCallback>(
          dlsym(handler_->handle_memory_, "HardThroughput")
      )
  };
}

Network Builder::BuildNetworkAgent() noexcept {
  assert(handler_->handle_network_);
  return {
      .url_available = reinterpret_cast<UrlAvailableCallback>(
          dlsym(handler_->handle_network_, "UrlAvailable")
      ),
      .inet_throughput = reinterpret_cast<InetThroughputCallback>(
          dlsym(handler_->handle_network_, "InetThroughput")
      )
  };
}

} // namespace monsys::agents