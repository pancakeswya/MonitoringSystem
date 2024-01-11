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
                            "Error: " + agent_status_map.at(stat);
    exc_callback_(error_str);
    return false;
  }
  return true;
}

template<typename Tp, typename Callback>
Tp Controller::HandleMetric(Callback callback) noexcept {
  static_assert(std::is_arithmetic_v<Tp>, "Must be arithmetic type");
  auto[stat, val] = callback;
  if (stat != MetricStatus::kOk) {
    std::string error_str = "Agent name: " + model_->ExecutedAgentName() +
                            "Agent type: " + model_->ExecutedAgentType() +
                            "Error: " + metric_status_map.at(stat);
    exc_callback_(error_str);
    return 0;
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

bool Controller::SetConfig(const std::string& config_path) {
  return HandleAgent(model_->SetConfig(config_path));
}

double Controller::CpuLoad() {
  return HandleMetric<double>(model_->CpuLoad());
}

size_t Controller::CpuProcesses() {
  return HandleMetric<size_t>(model_->CpuProcesses());
}

double Controller::RamTotal() {
  return HandleMetric<double>(model_->RamTotal());
}

double Controller::Ram() {
  return HandleMetric<double>(model_->Ram());
}

double Controller::HardVolume() {
  return HandleMetric<double>(model_->HardVolume());
}

size_t Controller::HardOps() {
  return HandleMetric<size_t>(model_->HardOps());
}

double Controller::HardThroughput() {
  return HandleMetric<double>(model_->HardThroughput());
}

double Controller::InetThroughput() {
  return HandleMetric<double>(model_->InetThroughput());
}

int Controller::UrlAvailable() {
  return HandleMetric<int>(model_->UrlAvailable());
}

void Controller::OnException(Controller::OnExceptionCallback callback) {
  exc_callback_ = callback;
}

void Controller::Reset() noexcept {
  model_->Reset();
}


} // namespace monsys