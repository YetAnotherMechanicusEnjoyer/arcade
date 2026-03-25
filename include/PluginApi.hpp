/*
** EPITECH PROJECT, 2026
** PluginApi
** File description:
** PluginApi
*/

#ifndef PLUGINAPI
  #define PLUGINAPI
  #include "Common.hpp"
 
namespace Arcade {
  using CreateFn = void* (*)();
  using DestroyFn = void (*)(void*);
  using GetTypeFn = PluginType (*)();
  using GetNameFn = const char* (*)();
}

#endif
