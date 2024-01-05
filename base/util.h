#ifndef MONITORINGSYSTEM_SRC_AGENTS_UTIL_H_
#define MONITORINGSYSTEM_SRC_AGENTS_UTIL_H_

#include <sys/types.h>

enum ByteUnits {
  kBytes = 0,
  kKiloBytes = 1,
  kMegaBytes = 2,
  kGigaBytes = 3
};

static inline double ConvertToByteUnit(double num, int to, int from) {
  for(;from < to; from++) {
    num /= 1024;
  }
  return num;
}

double ParseMetricSingleD(char path[const restrict], char name[const restrict]);
size_t ParseMetricSingleU(char path[const restrict], char name[const restrict]);

#endif // MONITORINGSYSTEM_SRC_AGENTS_UTIL_H_