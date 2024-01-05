#include "agents/memory.h"
#include "base/defs.h"
#include "base/util.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
  return used / total * 100.0;
}

double HardVolume() {
  return 0;
}

size_t HardOps() {
  return 0;
}

double HardThroughput() {
  return 0;
}