#ifndef MONITORINGSYSTEM_SRC_BASE_ERROR_H_
#define MONITORINGSYSTEM_SRC_BASE_ERROR_H_

#include <string>

namespace monsys {

class Error {
 public:
  explicit Error(std::string error) noexcept : error_(std::move(error)) {}

  [[nodiscard]] std::string Str() const noexcept { return error_; }

  explicit operator bool() const noexcept { return !error_.empty(); }

 private:
  std::string error_;
};

static inline const auto nullerr_t = Error("");

}  // namespace monsys

#endif  // MONITORINGSYSTEM_SRC_BASE_ERROR_H_