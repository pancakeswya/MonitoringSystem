#include "agents/network.h"
#include "agents/defs.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

enum NetStat {
  kRxBytes = 0,
  kRxPackets = 1,
  kRxErrors = 2,
  kRxDropped = 3,
  kRxFifoErrors = 4,
  kRxFrame = 5,
  kRxCompressed = 6,
  kMulticast = 7,
  kTxBytes = 8,
  kTxPackets = 9,
  kTxErrors = 10,
  kTxDropped = 11,
  kTxFifoErrors = 12,
  kTxFrame = 13,
  kTxCompressed = 14,
  kTxMulticast = 15,
  kStatsSize
};

int UrlAvailable(const char* url) {
  const char util[] = "wget --spider -q ";
  size_t util_len = strlen(util), url_len = strlen(url);
  char* command = (char*)malloc(util_len + url_len + 1);
  strcpy(command, util);
  strcpy(command + util_len, url);
  command[util_len + url_len] = '\n';
  int res = system(command);
  free(command);
  return res == 0;
}

static size_t GetTotalBytes() {
  FILE* stat_file = fopen("/proc/net/dev", "r");
  assert(stat_file != NULL);
  char* line = (char*)malloc(BUFFER_SIZE);
  assert(line != NULL);

  size_t bytes = BUFFER_SIZE;

  size_t stats[kStatsSize] = {};

  for(int i = 0; getline(&line, &bytes, stat_file) != -1; ++i) {
    if (i > 1) {
      const char* ptr = line;
      char *end = NULL;
      for(;!isalpha(*ptr); ++ptr)
        ;
      for(;!isspace(*ptr); ++ptr)
        ;
      for(int j = 0; j < kStatsSize; ++j) {
        stats[j] += strtoull(ptr, &end, 10);
        assert(ptr != end);
        ptr = end;
      }
    }
  }
  free(line);
  fclose(stat_file);

  return stats[kRxBytes] + stats[kTxBytes];
}

double InetThroughput() {
  size_t prev_bytes = GetTotalBytes();
  usleep(DELAY_MCS);
  size_t curr_bytes = GetTotalBytes();
  return (double)(curr_bytes - prev_bytes);
}