#ifndef MONITORINGSYSTEM_SRC_CONTROLLER_CONTROLLER_H_
#define MONITORINGSYSTEM_SRC_CONTROLLER_CONTROLLER_H_

#include <functional>
#include <string>
#include <unordered_map>

#include "base/types.h"

namespace monsys {

class Model;

class Controller {
 public:
  using LoadAgentsMap = std::unordered_map<std::string_view, bool>;
  using ErrorPredicate = std::function<void(const std::string&)>;

  explicit Controller(Model* model);

  void LogMetrics();
  void LoadConfig();
  void SaveConfig();
  void LoadAgents() noexcept;

  void SetConfig(const Config& config);

  Config GetConfig();
  Metrics CollectMetrics();

  void OnError(ErrorPredicate err_pred) noexcept;

 private:
  void HandleError(const std::string& err_str);
  Model* model_;
  ErrorPredicate err_pred_;
};

}  // namespace monsys

#endif  // MONITORINGSYSTEM_SRC_CONTROLLER_CONTROLLER_H_
