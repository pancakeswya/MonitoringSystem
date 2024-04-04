#ifndef MONITORINGSYSTEM_SRC_LOGGER_LOGGER_H_
#define MONITORINGSYSTEM_SRC_LOGGER_LOGGER_H_

#include <string_view>

#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <sstream>

namespace monsys {

class Logger {
 public:
  static constexpr char endlog = '\n';

  static Logger& Log(const std::string& path) {
    static Logger logger(path);
    logger.ofs_ << '[' << GetCurrentTime() << "] ";
    return logger;
  }

  template<typename Tp, typename = std::enable_if_t<std::is_arithmetic_v<Tp>>>
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

  explicit Logger(const std::string& path) {
    ofs_.open(path, std::fstream::app);
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

#endif // MONITORINGSYSTEM_SRC_LOGGER_LOGGER_H_