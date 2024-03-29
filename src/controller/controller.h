#ifndef MONITORINGSYSTEM_SRC_CONTROLLER_CONTROLLER_H_
#define MONITORINGSYSTEM_SRC_CONTROLLER_CONTROLLER_H_

#include <base/types.h>

namespace monsys {

class Model;

class Controller {
 public:
  explicit Controller(Model* model) noexcept;

  void LoadAgents() noexcept;
  void UpdateConfig(const SystemConfig& config);
  SystemConfig GetConfig() noexcept;

  Metrics GetMetrics() noexcept;

  void OnException(ExceptionCallback);
 private:
  Model* model_;
};

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_CONTROLLER_CONTROLLER_H_
