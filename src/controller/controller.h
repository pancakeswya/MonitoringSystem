#ifndef MONITORINGSYSTEM_SRC_CONTROLLER_CONTROLLER_H_
#define MONITORINGSYSTEM_SRC_CONTROLLER_CONTROLLER_H_

#include <string>
#include <functional>
namespace monsys {

class Model;
struct Metrics;

class Controller {
 public:
  using OnExceptionCallback = std::function<void(const std::string&)>;

  explicit Controller(Model* model) noexcept;

  bool LoadCpuAgent() noexcept;
  bool LoadMemoryAgent() noexcept;
  bool LoadNetworkAgent() noexcept;

  void UnloadCpuAgent() noexcept;
  void UnloadMemoryAgent() noexcept;
  void UnloadNetworkAgent() noexcept;

  bool SetConfig(const std::string& config_path);

  void UpdateMetrics();

  Metrics GetMetrics() noexcept;

  void OnException(OnExceptionCallback);
  void Reset() noexcept;

 private:
  template<typename Callback>
  bool HandleAgent(Callback callback) noexcept;

  Model* model_;
  OnExceptionCallback exc_callback_{};
};

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_CONTROLLER_CONTROLLER_H_