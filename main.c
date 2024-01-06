#include "agents/cpu.h"
#include "agents/memory.h"
#include <stdio.h>

int main() {
  printf("%lf %ld\n", CpuLoad(990000), CpuProcesses());
  printf("%lf %lf %lu %lf %lf\n", RamTotal(), Ram(), HardOps(), HardVolume(), HardThroughput());
}