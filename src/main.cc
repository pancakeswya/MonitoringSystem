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

  if (!loaded) {
    return 1;
  }

  std::cout << "cpu load: " << controller.CpuLoad() << std::endl;
  std::cout << "cpu processes: " << controller.CpuProcesses() << std::endl;

  controller.UnloadCpuAgent();

  std::cout << "ram total: " << controller.RamTotal() << std::endl;
  std::cout << "ram: " << controller.Ram() << std::endl;
  std::cout << "hard ops: " << controller.HardOps() << std::endl;
  std::cout << "hard volume: " << controller.HardVolume() << std::endl;
  std::cout << "hard throughput: " << controller.HardThroughput() << std::endl;

  controller.UnloadMemoryAgent();

  std::cout << "url available: " << controller.UrlAvailable() << std::endl;
  std::cout << "inet throughput: " << controller.InetThroughput() << std::endl;

  controller.UnloadNetworkAgent();

  return 0;
}