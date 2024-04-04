#include "network.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include <curl/easy.h>

enum NetStat {
  kNetStatRxBytes = 0,
  kNetStatRxPackets = 1,
  kNetStatRxErrors = 2,
  kNetStatRxDropped = 3,
  kNetStatRxFifoErrors = 4,
  kNetStatRxFrame = 5,
  kNetStatRxCompressed = 6,
  kNetStatMulticast = 7,
  kNetStatTxBytes = 8,
  kNetStatTxPackets = 9,
  kNetStatTxErrors = 10,
  kNetStatTxDropped = 11,
  kNetStatTxFifoErrors = 12,
  kNetStatTxFrame = 13,
  kNetStatTxCompressed = 14,
  kNetStatTxMulticast = 15,
  kNetStatSize
};

CoreError UrlAvailable(int* url_available, const char* url) {
  CURL *curl = curl_easy_init();
  if (!curl) {
    return kCoreErrorCurlInitFail;
  }
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
  CURLcode response = curl_easy_perform(curl);
  *url_available = (response == CURLE_OK);
  curl_easy_cleanup(curl);
  return kCoreErrorSuccess;
}

static CoreError TotalBytes(size_t* total_bytes) {
  FILE* stat_file = fopen("/proc/net/dev", "r");
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
  size_t stats[kNetStatSize] = {0};
  for(int i = 0; getline(&line, &bytes, stat_file) != -1; ++i) {
    if (i > 1) {
      const char* ptr = line;
      char *end = NULL;
      for(;!isalpha(*ptr); ++ptr)
        ;
      for(;!isspace(*ptr); ++ptr)
        ;
      for(int j = 0; j < kNetStatSize; ++j) {
        stats[j] += strtoull(ptr, &end, 10);
        if (ptr == end) {
          err = kCoreErrorCannotReadProcFile;
          goto cleanup;
        }
        ptr = end;
      }
    }
  }
  *total_bytes = stats[kNetStatRxBytes] + stats[kNetStatTxBytes];
cleanup:
  free(line);
  fclose(stat_file);
  return err;
}

CoreError InetThroughput(double* inet_throughput, unsigned int delay) {
  size_t prev_bytes, curr_bytes;
  CoreError err = TotalBytes(&prev_bytes);
  if (err != kCoreErrorSuccess) {
    return err;
  }
  usleep(delay);
  err = TotalBytes(&curr_bytes);
  if (err != kCoreErrorSuccess) {
    return err;
  }
  *inet_throughput = (double)(curr_bytes - prev_bytes);
  return kCoreErrorSuccess;
}