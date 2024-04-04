#include "agents/dll.h"

#include <dlfcn.h>

namespace monsys {

Error Dll::Load(const std::string& path) noexcept {
  handle_ = dlopen(path.data(), RTLD_LAZY);
  return LastError();
}

Error Dll::LastError() noexcept {
  const char* err = dlerror();
  if (err == nullptr) {
    return nullerr_t;
  }
  return Error(err);
}

void* Dll::Symbol(const std::string &func_name) const noexcept {
  return dlsym(handle_, func_name.data());
}

Dll::~Dll() {
  if (handle_) {
    dlclose(handle_);
  }
}

} // namespace monsys