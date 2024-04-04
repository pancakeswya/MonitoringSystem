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
  static struct EndLog {
   private:
    friend Logger;
    static constexpr char end = '\n';
  } endlog;

  static Logger& Log(const std::string& path) {
    static Logger logger;
    logger.ofs_.open(path, std::ofstream::app);
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

  friend void operator<<(Logger& logger, EndLog) {
     logger.ofs_ << "|" << EndLog::end;
     logger.ofs_.close();
  }

 private:
  std::ofstream ofs_;

  ~Logger() {
    if (ofs_.is_open()) {
      ofs_.close();
    }
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