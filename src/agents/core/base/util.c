#include "agents/core/base/util.h"
#include "agents/core/base/defs.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char* FindMetricLine(const char* restrict path, const char* restrict target, const size_t target_len) {
  FILE* file = fopen(path, "r");
  assert(file != NULL);

  char* line = (char*)malloc(BUFFER_SIZE * sizeof(char));
  assert(line != NULL);

  size_t bytes = BUFFER_SIZE;

  do {
    long int read = getline(&line, &bytes, file);
    assert(read > 0);
    unused(read);
  } while(strncmp(target, line, target_len) != 0);
  // skip and parse processes
  fclose(file);

  return line;
}

double ParseMetricSingleD(const char* restrict path, const char* restrict target) {
  const size_t target_len = strlen(target);
  char* line = FindMetricLine(path, target, target_len);
  double metric = strtod(line + target_len, NULL);
  free(line);
  return metric;
}

size_t ParseMetricSingleU(const char* restrict path, const char* restrict target) {
  const size_t target_len = strlen(target);
  char* line = FindMetricLine(path, target, target_len);
  size_t metric = strtoul(line + target_len, NULL, 10);
  free(line);
  return metric;
}