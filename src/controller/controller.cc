#include "controller/controller.h"

#include <utility>
#include "model/model.h"
#include "base/types.h"

namespace monsys {

Controller::Controller(Model* model) noexcept
  : model_(model) {}

template<typename Callback>
inline bool Controller::HandleAgent(Callback callback) noexcept {
  AgentResponse response = callback;
  if (response.status != AgentStatus::kOk) {
    std::string error_str = "Agent name: " + response.name +
                            "\nError: " + GetStatusString(response.status);
    exc_callback_(error_str);
    return false;
  }
  return true;
}

bool Controller::LoadCpuAgent() noexcept {
  return HandleAgent(model_->LoadCpuAgent());
}

bool Controller::LoadMemoryAgent() noexcept {
  return HandleAgent(model_->LoadMemoryAgent());
}

bool Controller::LoadNetworkAgent() noexcept {
  return HandleAgent(model_->LoadNetworkAgent());
}

void Controller::UnloadCpuAgent() noexcept {
  HandleAgent(model_->UnloadCpuAgent());
}

void Controller::UnloadMemoryAgent() noexcept {
  HandleAgent(model_->UnloadMemoryAgent());
}

void Controller::UnloadNetworkAgent() noexcept {
  HandleAgent(model_->UnloadNetworkAgent());
}

void Controller::UpdateMetrics() {
  MetricResponse response = model_->UpdateMetrics();
  if (response.status != MetricStatus::kOk) {
    exc_callback_("Agent name: " + response.name +
                  "\nAgent type: " + response.type +
                  "\nError: " + GetStatusString(response.status));
  }
}

bool Controller::SetConfig(const std::string& config_path) {
  return HandleAgent(model_->SetConfig(config_path));
}

double Controller::CpuLoad() {
  return model_->CpuLoad();
}

size_t Controller::CpuProcesses() {
  return model_->CpuProcesses();
}

double Controller::RamTotal() {
  return model_->RamTotal();
}

double Controller::Ram() {
  return model_->Ram();
}

double Controller::HardVolume() {
  return model_->HardVolume();
}

size_t Controller::HardOps() {
  return model_->HardOps();
}

double Controller::HardThroughput() {
  return model_->HardThroughput();
}

double Controller::InetThroughput() {
  return model_->InetThroughput();
}

int Controller::UrlAvailable() {
  return model_->UrlAvailable();
}

void Controller::OnException(Controller::OnExceptionCallback callback) {
  exc_callback_ = std::move(callback);
}

void Controller::Reset() noexcept {
  model_->Reset();
}


} // namespace monsys