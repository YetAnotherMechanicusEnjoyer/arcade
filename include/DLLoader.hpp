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
    if (!_handle) {
      const char* error = dlerror();
      throw ARCError(std::string("dlopen error: ") + (error ? error : "unknown error"));
    }
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
      _handle = other._handle = other._handle = nullptr;
    }
    return *this;
  }

  bool hasSymbol(const std::string& symbolName) const {
    void* sym = dlsym(_handle, symbolName.c_str());
    if (sym) return true;

    std::string macOSName = "_" + symbolName;
    sym = dlsym(_handle, macOSName.c_str());
    return sym != nullptr;
  }

  std::unique_ptr<T> getInstance(const std::string& entryPointName = "entryPoint") {
    dlerror();

    void* sym = dlsym(_handle, entryPointName.c_str());

    if (!sym) {
      std::string macOSName = "_" + entryPointName;
      sym = dlsym(_handle, macOSName.c_str());
    }

    const char *dlsym_error = dlerror();
    if (dlsym_error)
      throw ARCError(std::string("dlsym error: ") + dlsym_error);

    if (!sym)
      throw ARCError(std::string("cannot find symbol: ") + entryPointName);

    using EntryPointFunc = T* (*)();
    EntryPointFunc createFunc = reinterpret_cast<EntryPointFunc>(sym);

    if (!createFunc)
      throw ARCError(std::string("invalid function pointer for: ") + entryPointName);
    
    return std::unique_ptr<T>(createFunc());
  }
};

}
