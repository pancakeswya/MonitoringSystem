#include "memory.h"
#include "util.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/statvfs.h>

enum MemoryDiskStats {
  kMemoryDiskStatsDiskId = 0,
  kMemoryDiskStatsPartitionId = 1,
  kMemoryDiskStatsPartitionName = 2,
  kMemoryDiskStatsReadsCompleted = 3,
  kMemoryDiskStatsReadsMerged = 4,
  kMemoryDiskStatsSectorsRead = 5,
  kMemoryDiskStatsMsSpentReading = 6,
  kMemoryDiskStatsWritesCompleted = 7,
  kMemoryDiskStatsWritesMerged = 8,
  kMemoryDiskStatsSectorsWritten = 9,
  kMemoryDiskStatsMsSpentWriting = 10,
  kMemoryDiskStatsIosInProgress = 11,
  kMemoryDiskStatsMsSpentOnIos = 12,
  kMemoryDiskStatsWeightedMsSp = 13,
  kMemoryDiskStatsSize = 14
};

CoreError RamTotal(double* ram_total) {
  CoreError err = ParseMetric("/proc/meminfo", "MemTotal:", ram_total);
  if (err != kCoreErrorSuccess) {
    return err;
  }
  *ram_total = ConvertToByteUnit(*ram_total, kCoreBUnitBytes, kCoreBUnitKiloBytes);
  return kCoreErrorSuccess;
}

CoreError Ram(double* ram) {
  FILE* mem_file = fopen("/proc/meminfo", "r");
  if (!mem_file) {
    return kCoreErrorCannotOpenProcFile;
  }
  size_t bytes = BUFFER_SIZE;
  char* line = (char*)malloc(bytes * sizeof(char));
  if (!line) {
    fclose(mem_file);
    return kCoreErrorAllocFail;
  }

  double total = 0.0, used = 0.0;

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
  *ram = used / total * 100.0;
  return kCoreErrorSuccess;
}

CoreError HardVolume(double* hard_volume) {
  struct statvfs buffer;
  int ret = statvfs("/", &buffer);
  if (ret == -1) {
    return kCoreErrorCannotOpenProcFile;
  }
  const double total = (double)(buffer.f_blocks * buffer.f_bsize);
  const double available = (double)(buffer.f_bfree * buffer.f_bsize);
  *hard_volume = (available / total) * 100.0;
  return kCoreErrorSuccess;
}

CoreError HardOps(size_t* hard_ops) {
  FILE* stat_file = fopen("/proc/diskstats", "r");
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
  size_t stats[kMemoryDiskStatsSize] = {0};

  while(getline(&line, &bytes, stat_file) != -1) {
    const char* ptr = line;
    char *end = NULL;
    for(;!isalpha(*ptr); ++ptr)
      ;
    for(;!isspace(*ptr); ++ptr)
      ;
    for(int i = 0; i < kMemoryDiskStatsSize; ++i) {
      stats[i] += strtoull(ptr, &end, 10);
      if (ptr == end) {
        err = kCoreErrorCannotReadProcFile;
        goto cleanup;
      }
      ptr = end;
    }
  }
  const double iops = (double)(stats[kMemoryDiskStatsReadsCompleted] + stats[kMemoryDiskStatsWritesCompleted]);
  const double iops_time_s = (double)(stats[kMemoryDiskStatsMsSpentReading] + stats[kMemoryDiskStatsMsSpentWriting]) / 1000.0;
  *hard_ops = (size_t)(iops / iops_time_s);
cleanup:
  free(line);
  fclose(stat_file);
  return err;
}

CoreError HardThroughput(double* hard_throughput) {
  struct statvfs buffer;
  int ret = statvfs("/", &buffer);
  if (ret == -1) {
    return kCoreErrorCannotOpenProcFile;
  }
  size_t hard_ops;
  CoreError err = HardOps(&hard_ops);
  if (err != kCoreErrorSuccess) {
    return err;
  }
  *hard_throughput = (double)(hard_ops  * buffer.f_bsize);
  return kCoreErrorSuccess;
}