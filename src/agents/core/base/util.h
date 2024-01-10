#ifndef MONITORINGSYSTEM_SRC_AGENTS_CORE_UTIL_H_
#define MONITORINGSYSTEM_SRC_AGENTS_CORE_UTIL_H_

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

extern double ParseMetricSingleD(const char* restrict path, const char* restrict target);
extern size_t ParseMetricSingleU(const char* restrict path, const char* restrict target);

#endif // MONITORINGSYSTEM_SRC_AGENTS_CORE_UTIL_H_