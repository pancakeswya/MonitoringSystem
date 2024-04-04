#ifndef MONITORINGSYSTEM_SRC_AGENTS_CORE_INCLUDE_CPU_H_
#define MONITORINGSYSTEM_SRC_AGENTS_CORE_INCLUDE_CPU_H_

#include "defs.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
extern CoreError CpuLoad(double* load, unsigned int delay);
extern CoreError CpuProcesses(size_t* processes);
#ifdef __cplusplus
}
#endif

#endif // MONITORINGSYSTEM_SRC_AGENTS_CORE_INCLUDE_CPU_H_
