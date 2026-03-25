/*
** EPITECH PROJECT, 2026
** CORE
** File description:
** arcade
*/

#ifndef CORE
  #define CORE
  #define SUCCESS 0
  #define HELP 2026
  #define ERROR 84
  #define FAIL 1
  #include "IGraphic.hpp"
  #include "PluginApi.hpp"

namespace Arcade {
class Core {
public:
  explicit Core(const std::string &initalGraphicPath);
  ~Core();
  void run();

private:
  void loadGraphics(const std::string& path);
  void unloadGraphics();

private:
  std::string _graphicalPath;
  void* _graphicHandle;
  IGraphics* _graphics;
  DestroyFn _destroyGraphics;

};
}

#endif /* CORE */
