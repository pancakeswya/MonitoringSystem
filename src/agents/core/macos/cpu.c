#include "agents/core/cpu.h"
#include "agents/core/base/defs.h"

#include <mach/mach.h>
#include <sys/sysctl.h>
#include <unistd.h>

enum CPUParameters {
  kCpUser = 0,
  kCpSys = 1,
  kCpIdle = 2,
  kCpNice = 3,
  kCpStates = 4
};

static host_cpu_load_info_data_t GetCpuPercentage() {
  kern_return_t error;
  mach_msg_type_number_t count;
  host_cpu_load_info_data_t r_load;
  mach_port_t mach_port;

  count = HOST_CPU_LOAD_INFO_COUNT;
  mach_port = mach_host_self();
  error = host_statistics(mach_port, HOST_CPU_LOAD_INFO, (host_info_t)&r_load,
                          &count);

  if (error != KERN_SUCCESS) {
    return (host_cpu_load_info_data_t){};
  }

  return r_load;
}

double CpuLoad() {
  host_cpu_load_info_data_t load1 = GetCpuPercentage();
  usleep(DELAY_MCS);
  host_cpu_load_info_data_t load2 = GetCpuPercentage();

  size_t current_user = load1.cpu_ticks[kCpUser],
         current_system = load1.cpu_ticks[kCpSys],
         current_nice = load1.cpu_ticks[kCpNice],
         current_idle = load1.cpu_ticks[kCpIdle];

  size_t next_user = load2.cpu_ticks[kCpUser],
         next_system = load2.cpu_ticks[kCpSys],
         next_nice = load2.cpu_ticks[kCpNice],
         next_idle = load2.cpu_ticks[kCpIdle];

  size_t diff_user = next_user - current_user,
         diff_system = next_system - current_system,
         diff_nice = next_nice - current_nice,
         diff_idle = next_idle - current_idle;

  return (double)(diff_user + diff_system + diff_nice) /
         (double)(diff_user + diff_system + diff_nice + diff_idle) * 100.0;
}

size_t CpuProcesses() {
  size_t length;
  static int names[] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};
  int res = sysctl(names, sizeof(names) / sizeof(names[0]) - 1, NULL, &length,
                   NULL, 0);
  if (res == 0) {
    return length / sizeof(struct kinfo_proc);
  }
  return 0;
}