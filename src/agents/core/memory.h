#ifndef MONITORINGSYSTEM_SRC_AGENTS_CORE_MEMORY_H_
#define MONITORINGSYSTEM_SRC_AGENTS_CORE_MEMORY_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
  extern double RamTotal(unsigned int delay);
  extern double Ram(unsigned int delay);
  extern double HardVolume(unsigned int delay);
  extern size_t HardOps(unsigned int delay);
  extern double HardThroughput(unsigned int delay);
#ifdef __cplusplus
}
#endif

#endif // MONITORINGSYSTEM_SRC_AGENTS_CORE_MEMORY_H_