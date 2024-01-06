#include "agents/cpu.h"
#include "agents/memory.h"
#include "agents/network.h"
#include <stdio.h>

int main() {
  printf("%lf %ld\n", CpuLoad(), CpuProcesses());
  printf("%lf %lf %lu %lf %lf\n", RamTotal(), Ram(), HardOps(), HardVolume(), HardThroughput());
  printf("%d %lf\n", UrlAvailable("ya.ru"), InetThroughput());
}