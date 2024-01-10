#ifndef MONITORINGSYSTEM_SRC_AGENTS_CORE_CPU_H_
#define MONITORINGSYSTEM_SRC_AGENTS_CORE_CPU_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
  extern double CpuLoad(unsigned int delay);
  extern size_t CpuProcesses(unsigned int delay);
#ifdef __cplusplus
}
#endif

#endif // MONITORINGSYSTEM_SRC_AGENTS_CORE_CPU_H_
