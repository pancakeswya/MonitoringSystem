#include "controller/controller.h"
#include "model/model.h"

#include <iostream>
#include <string>

int main() {
  const std::string path = "/mnt/c/Users/niyaz/CLionProjects/MonitoringSystem/resources/conf.agents";
  monsys::Model model;
  monsys::Controller controller(&model);

  controller.OnException([](const std::string& error) {
    std::cout << error << std::endl;
  });

  controller.SetConfig(path);

  bool loaded = true;

  loaded &= controller.LoadCpuAgent();
  loaded &= controller.LoadMemoryAgent();
  loaded &= controller.LoadNetworkAgent();

  controller.UpdateMetrics();


  if (!loaded) {
    return 1;
  }

  auto metrics = controller.GetMetrics();

  std::cout << "cpu load: " << metrics.cpu_load << std::endl;
  std::cout << "cpu processes: " << metrics.cpu_processes << std::endl;

  controller.UnloadCpuAgent();

  std::cout << "ram total: " << metrics.ram_total << std::endl;
  std::cout << "ram: " << metrics.ram << std::endl;
  std::cout << "hard ops: " << metrics.hard_ops << std::endl;
  std::cout << "hard volume: " << metrics.hard_volume << std::endl;
  std::cout << "hard throughput: " << metrics.hard_throughput << std::endl;

  controller.UnloadMemoryAgent();

  std::cout << "url available: " << metrics.url_available << std::endl;
  std::cout << "inet throughput: " << metrics.inet_throughput << std::endl;

  controller.UnloadNetworkAgent();

  return 0;
}