#include "controller/controller.h"
#include "model/model.h"

#include <unordered_map>
#include <type_traits>

namespace monsys {

namespace {

const std::unordered_map<AgentStatus, std::string> agent_status_map = {
    {AgentStatus::kOk, "No exception"},
    {AgentStatus::kNotLoaded, "Agent not loaded"},
    {AgentStatus::kAlreadyActive, "Agent already active"},
    {AgentStatus::kInvalidDeactivate, "Invalid agent deactivation"}
};

const std::unordered_map<MetricStatus, std::string> metric_status_map = {
    {MetricStatus::kOk, "No exception"},
    {MetricStatus::kOutOfRange, "Metric is out of range"},
    {MetricStatus::kInvalidUrl, "Invalid url"}
};

} // namespace

Controller::Controller(Model* model) noexcept
  : model_(model) {}

template<typename Callback>
bool Controller::HandleAgent(Callback callback) noexcept {
  AgentStatus stat = callback;
  if (stat != AgentStatus::kOk) {
    std::string error_str = "Agent name: " + model_->ExecutedAgentName() +
                            "\nError: " + agent_status_map.at(stat);
    exc_callback_(error_str);
    return false;
  }
  return true;
}

template<typename Callback, typename return_type>
inline return_type Controller::HandleMetric(Callback callback) noexcept {
  static_assert(std::is_arithmetic_v<return_type>, "Must be arithmetic type");
  auto[stat, val] = callback;
  if (stat != MetricStatus::kOk) {
    std::string error_str = "Agent name: " + model_->ExecutedAgentName() +
                            "\nAgent type: " + model_->ExecutedAgentType() +
                            "\nError: " + metric_status_map.at(stat);
    exc_callback_(error_str);
    return {};
  }
  return val;
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

bool Controller::SetConfig(const std::string& config_path) {
  return HandleAgent(model_->SetConfig(config_path));
}

double Controller::CpuLoad() {
  return HandleMetric(model_->CpuLoad());
}

size_t Controller::CpuProcesses() {
  return HandleMetric(model_->CpuProcesses());
}

double Controller::RamTotal() {
  return HandleMetric(model_->RamTotal());
}

double Controller::Ram() {
  return HandleMetric(model_->Ram());
}

double Controller::HardVolume() {
  return HandleMetric(model_->HardVolume());
}

size_t Controller::HardOps() {
  return HandleMetric(model_->HardOps());
}

double Controller::HardThroughput() {
  return HandleMetric(model_->HardThroughput());
}

double Controller::InetThroughput() {
  return HandleMetric(model_->InetThroughput());
}

int Controller::UrlAvailable() {
  return HandleMetric(model_->UrlAvailable());
}

void Controller::OnException(Controller::OnExceptionCallback callback) {
  exc_callback_ = callback;
}

void Controller::Reset() noexcept {
  model_->Reset();
}


} // namespace monsys