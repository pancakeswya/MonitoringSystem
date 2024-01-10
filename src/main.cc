#include "model/model.h"

#include <iostream>
#include <string>

int main() {
  const std::string path = "/mnt/c/Users/niyaz/CLionProjects/MonitoringSystem/resources/conf.agents";
  monsys::Model model;
  model.SetConfig(path);

  using AgentStatus = monsys::agents::AgentStatus;
  AgentStatus stat;
  stat = model.LoadCpuAgent();
  if (stat != AgentStatus::kOk) {
    std::cout << "Cpu lib not loaded code: " << int(stat) << std::endl;
    return 1;
  }
  stat = model.LoadMemoryAgent();
  if (stat != AgentStatus::kOk) {
    std::cout << "Memory lib not loaded code: " << int(stat) << std::endl;
    return 1;
  }
  stat = model.LoadNetworkAgent();
  if (stat != AgentStatus::kOk) {
    std::cout << "Network lib not loaded code: " <<  int(stat) << std::endl;
    return 1;
  }

  using MetricStatus = monsys::MetricStatus;

  auto[load_stat, cpu_load] = model.CpuLoad();
  if (load_stat != MetricStatus::kOk) {
    std::cout << "Cpu load failed code: " << int(load_stat) << std::endl;
    return 1;
  }
  std::cout << "CPU load: " << cpu_load << std::endl;

  auto[proc_stat, proc] = model.CpuProcesses();
  if (proc_stat != MetricStatus::kOk) {
    std::cout << "Cpu proc failed code: " << int(proc_stat) << std::endl;
    return 1;
  }
  std::cout << "Cpu proc: " << proc << std::endl;

  auto[ram_stat, ram] = model.Ram();
  if (ram_stat != MetricStatus::kOk) {
    std::cout << "Ram failed code: " << int(ram_stat) << std::endl;
    return 1;
  }
  std::cout << "Ram: " << ram << std::endl;

  auto[ram_total_stat, ram_total] = model.RamTotal();
  if (ram_total_stat != MetricStatus::kOk) {
    std::cout << "Ram total failed code: " << int(ram_total_stat) << std::endl;
    return 1;
  }
  std::cout << "Ram total: " << ram_total << std::endl;

  auto[ops_stat, ops] = model.HardOps();
  if (ops_stat != MetricStatus::kOk) {
    std::cout << "Ops failed code: " << int(ops_stat) << std::endl;
    return 1;
  }
  std::cout << "Hard ops: " << ops << std::endl;

  auto[volume_stat, volume] = model.HardVolume();
  if (volume_stat != MetricStatus::kOk) {
    std::cout << "Volume failed code: " << int(volume_stat) << std::endl;
    return 1;
  }
  std::cout << "Hard volume: " << volume << std::endl;

  auto[h_throughput_stat, h_throughput] = model.HardThroughput();
  if (h_throughput_stat != MetricStatus::kOk) {
    std::cout << "Hard throughout failed code: " << int(h_throughput_stat) << std::endl;
    return 1;
  }
  std::cout << "Hard throughout: " << h_throughput << std::endl;

  auto[url_stat, url] = model.UrlAvailable();
  if (url_stat != MetricStatus::kOk) {
    std::cout << "Url available failed code: " << int(url_stat) << std::endl;
    return 1;
  }
  std::cout << "Url available: " << url << std::endl;

  auto[i_throughput_stat,i_throughput] = model.InetThroughput();
  if (i_throughput_stat != MetricStatus::kOk) {
    std::cout << "Inet throughput available failed code: " << int(i_throughput_stat) << std::endl;
    return 1;
  }
  std::cout << "Inet throughput: " << i_throughput << std::endl;
  return 0;
}