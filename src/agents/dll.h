#ifndef MONITORINGSYSTEM_SRC_AGENTS_DLL_H_
#define MONITORINGSYSTEM_SRC_AGENTS_DLL_H_

#include "base/error.h"

#include <string>
#include <utility>

namespace monsys {

class Dll {
 public:
  Error Load(const std::string &path) noexcept;

  template<typename Fn, typename FnType>
  [[nodiscard]] std::pair<FnType, Error> Get() const noexcept;

  ~Dll();
 private:
  static Error LastError() noexcept;

  [[nodiscard]] void *Symbol(const std::string &func_name) const noexcept;

  void *handle_;
};

template<typename Fn, typename FnType = typename Fn::type>
inline std::pair<FnType, Error> Dll::Get() const noexcept {
  return {reinterpret_cast<FnType>(Symbol(Fn::kName.data())), Dll::LastError()};
}

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_AGENTS_DLL_H_