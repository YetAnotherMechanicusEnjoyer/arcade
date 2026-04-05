#pragma once

#include <string>
#include <memory>
#include <dlfcn.h>
#include "Errors.hpp"

namespace Arcade {

template <typename T>
class DLLoader {
private:
  void *_handle;

public:
  explicit DLLoader(const std::string& filepath) : _handle(nullptr) {
    _handle = dlopen(filepath.c_str(), RTLD_LAZY);
    if (!_handle)
      throw ARCError(dlerror());
  }

  ~DLLoader() {
    if (_handle)
      dlclose(_handle);
  }

  DLLoader(const DLLoader&) = delete;
  DLLoader& operator=(const DLLoader&) = delete;

  DLLoader(DLLoader&& other) noexcept : _handle(other._handle) {
    other._handle = nullptr;
  }

  DLLoader& operator=(DLLoader&& other) noexcept {
    if (this != &other) {
      if (_handle)
        dlclose(_handle);
      _handle = other._handle = nullptr;
    }
    return *this;
  }

  bool hasSymbol(const std::string& symbolName) const {
    dlerror();
    bool _ = dlsym(_handle, symbolName.c_str());
    return dlerror() == nullptr;
  }

  std::unique_ptr<T> getInstance(const std::string& entryPointName = "entryPoint") {
    dlerror();

    void* sym = dlsym(_handle, entryPointName.c_str());

    const char *dlsym_error = dlerror();
    if (dlsym_error)
      throw ARCError(std::string("dlsym error : ") + dlsym_error);

    using EntryPointFunc = T* (*)();
    EntryPointFunc createFunc = reinterpret_cast<EntryPointFunc>(sym);

    return std::unique_ptr<T>(createFunc());
  }
};
}
