#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

CoreError ParseMetric(const char* restrict path, const char* restrict target, double* metric) {
  FILE* file = fopen(path, "r");
  if (!file) {
    return kCoreErrorCannotOpenProcFile;
  }
  CoreError err = kCoreErrorSuccess;
  size_t bytes = BUFFER_SIZE;
  char* line = (char*)malloc(bytes * sizeof(char));
  if (!line) {
    err = kCoreErrorAllocFail;
    goto cleanup;
  }
  const size_t target_len = strlen(target);
  do {
    long int read = getline(&line, &bytes, file);
    if (read == -1) {
      err = kCoreErrorCannotReadProcFile;
      goto cleanup;
    }
  } while(strncmp(target, line, target_len) != 0);
  // skip and parse processes
  *metric = strtod(line + target_len, NULL);
cleanup:
  free(line);
  fclose(file);
  return err;
}