#ifndef MONITORINGSYSTEM_SRC_AGENTS_CORE_SRC_UTIL_H_
#define MONITORINGSYSTEM_SRC_AGENTS_CORE_SRC_UTIL_H_

#include "defs.h"

#include <sys/types.h>

static inline double ConvertToByteUnit(double num, int to, int from) {
  for(;from < to; from++) {
    num /= 1024;
  }
  return num;
}

extern CoreError ParseMetric(const char* restrict path, const char* restrict target, double* metric);

#endif // MONITORINGSYSTEM_SRC_AGENTS_CORE_SRC_UTIL_H_