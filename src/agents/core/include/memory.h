#ifndef MONITORINGSYSTEM_SRC_AGENTS_CORE_INCLUDE_MEMORY_H_
#define MONITORINGSYSTEM_SRC_AGENTS_CORE_INCLUDE_MEMORY_H_

#include <stddef.h>

#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif
extern CoreError RamTotal(double* ram_total);
extern CoreError Ram(double* ram);
extern CoreError HardVolume(double* hard_volume);
extern CoreError HardOps(size_t* hard_ops);
extern CoreError HardThroughput(double* hard_throughput);
#ifdef __cplusplus
}
#endif

#endif  // MONITORINGSYSTEM_SRC_AGENTS_CORE_INCLUDE_MEMORY_H_