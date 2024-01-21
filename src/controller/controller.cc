#include "controller/controller.h"
#include "model/model.h"

namespace monsys {

Controller::Controller(Model* model) noexcept
  : model_(model) {}

void Controller::LoadAgents() noexcept {
  model_->LoadAgents();
}

void Controller::UpdateConfig(const SystemConfig& config) {
  model_->UpdateConfig(config);
}

Metrics Controller::GetMetrics() noexcept {
  model_->UpdateMetrics();
  return model_->GetMetrics();
}

void Controller::OnException(ExceptionCallback callback) {
  model_->SetExceptionCallback(std::move(callback));
}

} // namespace monsys