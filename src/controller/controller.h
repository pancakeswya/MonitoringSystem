#ifndef MONITORINGSYSTEM_SRC_CONTROLLER_CONTROLLER_H_
#define MONITORINGSYSTEM_SRC_CONTROLLER_CONTROLLER_H_

#include <string>
#include <functional>

namespace monsys {

class Model;

class Controller {
 public:
  using OnExceptionCallback = std::function<void(const std::string&)>;

  explicit Controller(Model* model) noexcept;

  bool LoadCpuAgent() noexcept;
  bool LoadMemoryAgent() noexcept;
  bool LoadNetworkAgent() noexcept;
  bool SetConfig(const std::string& config_path);

  double CpuLoad();
  size_t CpuProcesses();
  double RamTotal();
  double Ram();
  double HardVolume();
  size_t HardOps();
  double HardThroughput();
  double InetThroughput();
  int UrlAvailable();

  void OnException(OnExceptionCallback);
  void Reset() noexcept;

 private:
  template<typename Callback>
  bool HandleAgent(Callback callback) noexcept;

  template<typename Callback, typename return_type = typename Callback::second_type>
  return_type HandleMetric(Callback callback) noexcept;

  Model* model_;
  OnExceptionCallback exc_callback_{};
};

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_CONTROLLER_CONTROLLER_H_