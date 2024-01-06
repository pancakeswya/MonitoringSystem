#include "agents/memory.h"
#include "agents/defs.h"
#include "agents/util.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/statvfs.h>

enum DiskStats {
  kDiskId = 0,
  kPartitionId = 1,
  kPartitionName = 2,
  kReadsCompleted = 3,
  kReadsMerged = 4,
  kSectorsRead = 5,
  kMsSpentReading = 6,
  kWritesCompleted = 7,
  kWritesMerged = 8,
  kSectorsWritten = 9,
  kMsSpentWriting = 10,
  kIosInProgress = 11,
  kMsSpentOnIos = 12,
  kWeightedMsSp = 13,
  kStatsSize = 14
};

double RamTotal() {
  double ram_total = ParseMetricSingleD("/proc/meminfo", "MemTotal:");
  return ConvertToByteUnit(ram_total, kMegaBytes, kKiloBytes);
}

double Ram() {
  FILE* mem_file = fopen("/proc/meminfo", "r");
  assert(mem_file != NULL);
  char* line = (char*)malloc(BUFFER_SIZE);
  assert(line != NULL);

  size_t bytes = BUFFER_SIZE;

  double total, used;

  const size_t mem_total_len = strlen("MemTotal:"),
               mem_free_len = strlen("MemFree:"),
               shmem_len = strlen("Shmem:"),
               buffers_len = strlen("Buffers:"),
               cached_len = strlen("Cached:"),
               sreclaimable_len = strlen("SReclaimable:");

  while(getline(&line, &bytes, mem_file) != -1) {
    if (strncmp(line, "MemTotal:", mem_total_len) == 0) {
      total = strtod(line + mem_total_len, NULL);
    } else if (strncmp(line, "MemFree:", mem_free_len) == 0) {
      used = total - strtod(line + mem_free_len, NULL);
    } else if (strncmp(line, "Shmem:", shmem_len) == 0) {
      used += strtod(line + shmem_len, NULL);
    } else if (strncmp(line, "Buffers:", buffers_len) == 0) {
      used -= strtod(line + buffers_len, NULL);
    } else if (strncmp(line, "Cached:", cached_len) == 0) {
      used -= strtod(line + cached_len, NULL);
    } else if (strncmp(line, "SReclaimable:", sreclaimable_len) == 0) {
      used -= strtod(line + sreclaimable_len, NULL);
    }
  }
  free(line);
  fclose(mem_file);

  return used / total * 100.0;
}

double HardVolume() {
  struct statvfs buffer;
  int ret = statvfs("/", &buffer);
  assert(ret != -1);
  unused(ret);
  const double total = (double)(buffer.f_blocks * buffer.f_bsize);
  const double available = (double)(buffer.f_bfree * buffer.f_bsize);
  return (available / total) * 100.0;
}

size_t HardOps() {
  FILE* stat_file = fopen("/proc/diskstats", "r");
  assert(stat_file != NULL);
  char* line = (char*)malloc(BUFFER_SIZE);
  assert(line != NULL);

  size_t bytes = BUFFER_SIZE;
  size_t stats[kStatsSize] = {};

  while(getline(&line, &bytes, stat_file) != -1) {
    const char* ptr = line;
    char *end = NULL;
    for(;!isalpha(*ptr); ++ptr)
      ;
    for(;!isspace(*ptr); ++ptr)
      ;
    for(int i = 0; i < kStatsSize; ++i) {
      stats[i] += strtoull(ptr, &end, 10);
      assert(ptr != end);
      ptr = end;
    }
  }
  free(line);
  fclose(stat_file);

  const double iops = (double)(stats[kReadsCompleted] + stats[kWritesCompleted]);
  const double iops_time_s = (double)(stats[kMsSpentReading] + stats[kMsSpentWriting]) / 1000.0;

  return (size_t)(iops / iops_time_s);
}

double HardThroughput() {
  struct statvfs buffer;
  int ret = statvfs("/", &buffer);
  assert(ret != -1);
  unused(ret);
  return HardOps() * buffer.f_bsize;
}