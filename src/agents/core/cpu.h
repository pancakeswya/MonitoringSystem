#ifndef MONITORINGSYSTEM_SRC_AGENTS_CORE_CPU_H_
#define MONITORINGSYSTEM_SRC_AGENTS_CORE_CPU_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
  double CpuLoad();
  size_t CpuProcesses();
#ifdef __cplusplus
}
#endif

#endif // MONITORINGSYSTEM_SRC_AGENTS_CORE_CPU_H_
