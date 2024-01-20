#ifndef MONITORINGSYSTEM_SRC_BASE_LOGGER_H_
#define MONITORINGSYSTEM_SRC_BASE_LOGGER_H_

#include "base/paths.h"

#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <sstream>

namespace monsys {

class Logger {
 public:
  static constexpr char endlog = '\n';

  static Logger& Log() {
    static Logger logger;
    logger.ofs_ << '[' << GetCurrentTime() << "] ";
    return logger;
  }

  template<typename Tp, typename = std::enable_if_t<std::is_arithmetic_v<Tp>, bool>>
  friend Logger& operator<<(Logger& logger, Tp val) {
    logger.ofs_ << ": " << val << ' ';
    return logger;
  }

  friend Logger& operator<<(Logger& logger, const std::string& metric) {
    logger.ofs_ << "| " << metric << ' ';
    return logger;
  }

  friend Logger& operator<<(Logger& logger, char symbol) {
    logger.ofs_ << "|" << symbol;
    return logger;
  }

 private:
  std::ofstream ofs_;

  Logger() {
    ofs_.open(paths::kLogsFile, std::fstream::app);
  }

  ~Logger() {
    ofs_.close();
  }

  static std::string GetCurrentTime() {
    auto t = std::time(nullptr);
    auto tm = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
  }
};

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_BASE_LOGGER_H_