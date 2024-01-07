#include "agents/handler.h"
#include "agents/builder.h"

#include <iostream>

int main() {
  monsys::agents::Handler handler;
  monsys::agents::Builder builder(&handler);
  handler.ActivateCpuAgent();
  auto cpu_agent = builder.BuildCpuAgent();
  std::cout << cpu_agent.cpu_load() << ' ' << cpu_agent.cpu_process() << std::endl;
  handler.ActivateMemoryAgent();
  auto memory_agent = builder.BuildMemoryAgent();
  std::cout << memory_agent.ram_total() << ' ' << memory_agent.ram() << ' '
            << memory_agent.hard_volume() << ' ' << memory_agent.hard_ops() << ' '
            << memory_agent.hard_throughput() << std::endl;
  handler.ActivateNetworkAgent();
  auto network_agent = builder.BuildNetworkAgent();
  std::cout << network_agent.url_available("ya.ru") << ' ' << network_agent.inet_throughput() << std::endl;

}