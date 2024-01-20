#include "controller/controller.h"
#include "model/model.h"

namespace monsys {

Controller::Controller(Model* model) noexcept
  : model_(model) {}

void Controller::LoadAgents() noexcept {
  model_->LoadAgents();
}

Metrics Controller::GetMetrics() noexcept {
  model_->UpdateMetrics();
  return model_->GetMetrics();
}

void Controller::OnException(OnExceptionCallback callback) {
  model_->SetExceptionCallback(std::move(callback));
}

} // namespace monsys