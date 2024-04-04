#include "cpu.h"
#include "util.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

enum CpuParams {
  kCpuParamsUser = 0,
  kCpuParamsNice = 1,
  kCpuParamsSys = 2,
  kCpuParamsIdle = 3,
  kCpuParamsStates = 4
};

static CoreError ReadStat(unsigned long long int stats[kCpuParamsStates]) {
  FILE* stat_file = fopen("/proc/stat", "r");
  if (!stat_file) {
    return kCoreErrorCannotOpenProcFile;
  }
  CoreError err = kCoreErrorSuccess;
  size_t bytes = BUFFER_SIZE;
  char* line = (char*)malloc(bytes * sizeof(char));
  if (!line) {
    err = kCoreErrorAllocFail;
    goto cleanup;
  }
  long int read = getline(&line, &bytes, stat_file);
  if (read == -1) {
    err = kCoreErrorCannotReadProcFile;
    goto cleanup;
  }
  // skip "cpu"
  char* line_ptr = line + 3, *end = NULL;
  for(int i = 0; i < kCpuParamsStates; ++i) {
    stats[i] = strtoll(line_ptr,&end, 10);
    line_ptr = end;
  }
cleanup:
  free(line);
  fclose(stat_file);
  return err;
}

CoreError CpuLoad(double* load, unsigned int delay) {
  unsigned long long int prev_stats[kCpuParamsStates],
                         curr_stats[kCpuParamsStates],
                         result_stats[kCpuParamsStates];
  CoreError err = ReadStat(prev_stats);
  if (err != kCoreErrorSuccess) {
    return err;
  }
  usleep(delay);
  err = ReadStat(curr_stats);
  if (err != kCoreErrorSuccess) {
    return err;
  }
  for(int i = 0; i < kCpuParamsStates; ++i) {
    result_stats[i] = curr_stats[i] - prev_stats[i];
  }
  *load = (double)(result_stats[kCpuParamsUser] + result_stats[kCpuParamsNice] + result_stats[kCpuParamsSys])
        / (double)(result_stats[kCpuParamsUser] + result_stats[kCpuParamsNice] + result_stats[kCpuParamsSys] + result_stats[kCpuParamsIdle])
        * 100.0;
  return kCoreErrorSuccess;
}

CoreError CpuProcesses(size_t* processes) {
  double res;
  CoreError err = ParseMetric("/proc/stat", "processes", &res);
  if (err != kCoreErrorSuccess) {
    return err;
  }
  *processes = (size_t)res;
  return kCoreErrorSuccess;
}