#ifndef MONITORINGSYSTEM_SRC_BASE_LOGGER_H_
#define MONITORINGSYSTEM_SRC_BASE_LOGGER_H_

#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <sstream>

namespace monsys {

template <typename T, typename...U>
using is_all_same = std::integral_constant<bool, (... && std::is_same_v<T,U>)>;

class Logger {
 public:
  static constexpr char endlog = '\n';

  static std::ofstream& Instance(const std::string& path) {
    static std::ofstream logger;
    if (logger.is_open()) {
      logger.close();
    }
    logger.open(path, std::fstream::app);
    return logger;
  }

  static std::ofstream& Log(const std::string& path) {
    std::ofstream& logger = Instance(path);
    logger << GetCurrentTime() << ' ';
    return logger;
  }

  template<typename ...Args, typename = std::enable_if_t<(std::is_same_v<const std::string&, Args> && ...)>>
  static void LogAll(const std::string& message, Args&&... paths) {
    std::string time = GetCurrentTime();
    ([=]
    {
      std::ofstream &logger = Instance(paths);
      logger << time << ' ' << message << endlog;
    } (), ...);
  }

 private:
  Logger() {}

  static std::string GetCurrentTime() {
    auto t = std::time(nullptr);
    auto tm = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d-%H:%M:%S");
    return oss.str();
  }
};

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_BASE_LOGGER_H_