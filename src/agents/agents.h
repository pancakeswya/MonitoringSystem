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
  CpuLoadCallback cpu_load;
  CpuProcessCallback cpu_process;
};

struct Memory {
  RamTotalCallback ram_total;
  RamCallback ram;
  HardVolumeCallback hard_volume;
  HardOpsCallback hard_ops;
  HardThroughputCallback hard_throughput;
};

struct Network {
  UrlAvailableCallback url_available;
  InetThroughputCallback inet_throughput;
};

} // namespace monsys::agents


#endif // MONITORINGSYSTEM_SRC_AGENTS_AGENTS_H_