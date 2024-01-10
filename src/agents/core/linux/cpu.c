#include "agents/core/cpu.h"
#include "agents/core/base/defs.h"
#include "agents/core/base/util.h"

#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum CPUParameters {
  kUser = 0,
  kNice = 1,
  kSys = 2,
  kIdle = 3,
  kStates = 4
};

static void ReadStat(unsigned long long int stats[kStates]) {
  FILE* stat_file = fopen( "/proc/stat", "r");
  assert(stat_file != NULL);

  char* line = (char*)malloc(BUFFER_SIZE);
  assert(line != NULL);

  size_t bytes = BUFFER_SIZE;

  long int read = getline(&line, &bytes, stat_file);
  assert(read != -1);
  unused(read);
  fclose(stat_file);
  // skip "cpu"
  char* line_ptr = line + 3;
  char *end = NULL;
  for(int i = 0; i < kStates; ++i) {
    stats[i] = strtoll(line_ptr,&end, 10);
    line_ptr = end;
  }
  free(line);
}

double CpuLoad(unsigned int delay) {
  unsigned long long int prev_stats[kStates],
                         curr_stats[kStates],
                         result_stats[kStates];

  ReadStat(prev_stats);
  usleep(delay);
  ReadStat(curr_stats);

  for(int i = 0; i < kStates; ++i) {
    result_stats[i] = curr_stats[i] - prev_stats[i];
  }

  return (double)(result_stats[kUser] + result_stats[kNice] + result_stats[kSys])
       / (double)(result_stats[kUser] + result_stats[kNice] + result_stats[kSys] + result_stats[kIdle])
       * 100.0;
}

size_t CpuProcesses(unsigned int delay) {
  usleep(delay);
  return ParseMetricSingleU("/proc/stat", "processes");
}