#ifndef MONITORINGSYSTEM_SRC_AGENTS_AGENTS_H_
#define MONITORINGSYSTEM_SRC_AGENTS_AGENTS_H_

#include "agents/core/cpu.h"
#include "agents/core/memory.h"
#include "agents/core/network.h"

namespace monsys::agents {

using CpuLoadCallback = typeof(&CpuLoad);
using CpuProcessCallback = typeof(&CpuProcesses);

using RamTotalCallback = typeof(&RamTotal);
using RamCallback = typeof(&Ram);
using HardVolumeCallback = typeof(&HardVolume);
using HardOpsCallback = typeof(&HardOps);
using HardThroughputCallback = typeof(&HardThroughput);

using UrlAvailableCallback = typeof(&UrlAvailable);
using InetThroughputCallback = typeof(&InetThroughput);

struct CPU {
  const CpuLoadCallback cpu_load;
  const CpuProcessCallback cpu_process;
};

struct Memory {
  const RamTotalCallback ram_total;
  const RamCallback ram;
  const HardVolumeCallback hard_volume;
  const HardOpsCallback hard_ops;
  const HardThroughputCallback hard_throughput;
};

struct Network {
  const UrlAvailableCallback url_available;
  const InetThroughputCallback inet_throughput;
};

} // namespace monsys::agents


#endif // MONITORINGSYSTEM_SRC_AGENTS_AGENTS_H_