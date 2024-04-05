#include "controller/controller.h"

#include <utility>

#include "model/model.h"

namespace monsys {

Controller::Controller(Model* model) : model_(model) {}

void Controller::HandleError(const std::string& err_str) {
  if (err_pred_) {
    err_pred_(err_str);
  }
}

void Controller::LogMetrics() { model_->LogMetrics(); }

void Controller::LoadConfig() {
  if (Error err = model_->LoadConfig(); err) {
    HandleError(err.Str());
    return;
  }
}

void Controller::SaveConfig() {
  if (Error err = model_->SaveConfig(); err) {
    HandleError(err.Str());
  }
}

void Controller::LoadAgents() noexcept {
  Model::ErrorMap errors = model_->LoadAgents();
  for (auto& [at, err] : errors) {
    if (err) {
      HandleError(std::string("error occurred at: ") + at.data() +
                  " with message: " + err.Str());
    }
  }
}

void Controller::SetConfig(const Config& config) { model_->SetConfig(config); }

Config Controller::GetConfig() { return model_->GetConfig(); }

Metrics Controller::CollectMetrics() {
  Model::ErrorMap errors = model_->UpdateMetrics();
  for (auto& [at, err] : errors) {
    if (err) {
      HandleError(std::string("error occurred at: ") + at.data() +
                  " with message: " + err.Str());
    }
  }
  return model_->GetMetrics();
}

void Controller::OnError(ErrorPredicate err_pred) noexcept {
  err_pred_ = std::move(err_pred);
}

}  // namespace monsys
