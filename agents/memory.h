#ifndef MONITORINGSYSTEM_SRC_AGENTS_MEMORY_H_
#define MONITORINGSYSTEM_SRC_AGENTS_MEMORY_H_

//[double] общий объем оперативной памяти (ram_total)
//[double] загрузка оперативной памяти в процентах (ram),
//[double] использование объема жесткого диска (hard_volume),
//[int] количество операций ввода-вывода для жесткого диска в секунду (hard_ops)
//[double] пропускная способность жесткого диска (hard_throughput)

#include <sys/types.h>

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

#endif // MONITORINGSYSTEM_SRC_AGENTS_MEMORY_H_