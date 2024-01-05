#include "base/util.h"
#include "base/defs.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char* FindMetricLine(char path[const restrict], char target[const restrict], const size_t target_len) {
  FILE* file = fopen(path, "r");
  assert(mem_file != NULL);

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

double ParseMetricSingleD(char path[const restrict], char target[const restrict]) {
  const size_t target_len = strlen(target);
  char* line = FindMetricLine(path, target, target_len);
  double metric = strtod(line + target_len, NULL);
  free(line);
  return metric;
}

size_t ParseMetricSingleU(char path[const restrict], char target[const restrict]) {
  const size_t target_len = strlen(target);
  char* line = FindMetricLine(path, target, target_len);
  size_t metric = strtoul(line + target_len, NULL, 10);
  free(line);
  return metric;
}