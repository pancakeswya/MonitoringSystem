#ifndef MONITORINGSYSTEM_SRC_AGENTS_CPU_H_
#define MONITORINGSYSTEM_SRC_AGENTS_CPU_H_

#include <sys/types.h>

#ifdef __OpenBSD__
#include <sys/sched.h>
#endif

enum CPUParameters {
#if defined(__APPLE__) && defined(__MACH__)
  kUser = 0,
  kSys = 1,
  kIdle = 2,
  kNice = 3,
  kStates = 4
#elif defined(__OpenBSD__)
  kStates = CPUSTATES
#else
  kUser = 0,
  kNice = 1,
  kSys = 2,
#if defined(__FreeBSD__) || defined(__NetBSD__)
// *BSD or OSX
  kIntr = 3,
  kIdle = 4,
  kStates = 5
#else
//linux
  kIdle = 3,
  kStates = 4
#endif
#endif
};

#ifdef __cplusplus
extern "C" {
#endif
  double CpuLoad(unsigned int delay);
  size_t CpuProcesses();
#ifdef __cplusplus
}
#endif

#endif // MONITORINGSYSTEM_SRC_AGENTS_CPU_H_
