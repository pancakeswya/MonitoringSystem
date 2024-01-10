#include "agents/core/memory.h"
#include "agents/core/base/defs.h"
#include "agents/core/base/util.h"

#include <assert.h>
#include <libproc.h>
#include <mach/mach.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include <sys/sysctl.h>
#include <unistd.h>

static vm_statistics_data_t GetRamVmStat() {
  static int mib[] = {CTL_HW, HW_MEMSIZE};
  int64_t value = 0;
  size_t length = sizeof(value);

  int res = sysctl(mib, 2, &value, &length, NULL, 0);
  assert(res == 0);
  mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
  vm_statistics_data_t vmstat;
  res = host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vmstat,
                        &count);
  assert(res == KERN_SUCCESS);
  unused(res);
  return vmstat;
}

static size_t GetIOBytes() {
  int n = proc_listpids(PROC_ALL_PIDS, 0, NULL, 0);

  int *pids = (int *)malloc(n * sizeof(int));
  assert(pids != NULL);
  int k = proc_listpids(PROC_ALL_PIDS, 0, pids, n * sizeof(int));

  size_t bytes = 0;
  for (int i = 0; i < k; ++i) {
    rusage_info_current rusage;
    int res = proc_pid_rusage(pids[i], RUSAGE_INFO_CURRENT, (void **)&rusage);
    if (res == 0) {
      bytes += rusage.ri_diskio_bytesread + rusage.ri_diskio_byteswritten;
    }
  }
  free(pids);
  return bytes;
}

double HardVolume(unsigned int delay) {
  usleep(delay);
  struct statvfs buffer;
  int ret = statvfs("/", &buffer);
  assert(ret != -1);
  unused(ret);
  const double total = (double)(buffer.f_blocks * buffer.f_bsize);
  const double available = (double)(buffer.f_bfree * buffer.f_bsize);
  return (available / total) * 100.0;
}

size_t HardOps(unsigned int delay) {
  size_t prev_bytes = GetIOBytes();
  usleep(delay);
  size_t curr_bytes = GetIOBytes();
  return curr_bytes - prev_bytes;
}

double HardThroughput(unsigned int delay)
  struct statvfs buffer;
  int ret = statvfs("/", &buffer);
  assert(ret != -1);
  unused(ret);
  return HardOps(delay) * buffer.f_bsize;
}

double RamTotal(unsigned int delay) {
  usleep(delay);
  const vm_statistics_data_t vmstat = GetRamVmStat();
  const double total = vmstat.wire_count + vmstat.active_count +
                       vmstat.inactive_count + vmstat.free_count;
  return ConvertToByteUnit(total, kMegaBytes, kKiloBytes);
}

double Ram(unsigned int delay) {
  usleep(delay);
  const vm_statistics_data_t vmstat = GetRamVmStat();
  const double total = vmstat.wire_count + vmstat.active_count +
                       vmstat.inactive_count + vmstat.free_count;
  const double free = vmstat.free_count / total;
  return (total - free) / total * 100.0;
}