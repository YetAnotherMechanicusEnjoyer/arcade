/*
** EPITECH PROJECT, 2026
** core
** File description:
** core
*/

#include <cstddef>
#include <memory>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <dlfcn.h>
#include "Core.hpp"

namespace Arcade {
  Core::Core(const std::string& graphicalPath)
  : _graphicalPath(graphicalPath),
    _graphicHandle(nullptr),
    _graphics(nullptr),
    _destroyGraphics(nullptr)
  {}

  Core::~Core(){
    unloadGraphics();
  }

  void Core::loadGraphics(const std::string& path){
    // open graphic lib
    _graphicHandle = dlopen(path.c_str(), RTLD_LAZY);
    if (!_graphicHandle)
      throw std::runtime_error(dlerror());

    // load getType function of the lib 
    auto getType = reinterpret_cast<GetTypeFn>(dlsym(_graphicHandle, "getType"));
    const char *error = dlerror();
    if (error)
      throw std::runtime_error(error);

    // load the create function of the lib
    auto create = reinterpret_cast<CreateFn>(dlsym(_graphicHandle, "create"));
    error = dlerror();
    if (error)
      throw std::runtime_error(error);

    //load the destroy fun of the lib
    _destroyGraphics = reinterpret_cast<DestroyFn>(dlsym(_graphicHandle, "destroy"));
    error = dlerror();
    if (error)
      throw std::runtime_error(error);

    // Error Handling for wrong lib
    if (getType() != PluginType::Graphics)
    throw std::runtime_error("'" + path + "' is not a graphical library");

    // create _graphics object for core use
    _graphics = static_cast<IGraphics*>(create());
    if (!_graphics)
      throw std::runtime_error("failed to create graphics instance");
  }

  void Core::unloadGraphics()
  {
    if (_graphics && _destroyGraphics) {
      _destroyGraphics(_graphics);
      _graphics = nullptr;
    }
    _destroyGraphics = nullptr;
    if (_graphicHandle) {
      dlclose(_graphicHandle);
      _graphicHandle = nullptr;
    }
  }

  void Core::run() {
    loadGraphics(_graphicalPath);
    _graphics->init();

    //temporary
    std::cout << "Graphics library loaded successfully\n";

    _graphics->shutdown();
  }
} 
