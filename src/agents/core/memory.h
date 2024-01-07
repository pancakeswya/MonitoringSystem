#ifndef MONITORINGSYSTEM_SRC_AGENTS_CORE_MEMORY_H_
#define MONITORINGSYSTEM_SRC_AGENTS_CORE_MEMORY_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
  double RamTotal();
  double Ram();
  double HardVolume();
  size_t HardOps();
  double HardThroughput();
#ifdef __cplusplus
}
#endif

#endif // MONITORINGSYSTEM_SRC_AGENTS_CORE_MEMORY_H_