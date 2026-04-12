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
  #include "IGame.hpp"
  #include "IGraphic.hpp"
  #include "DLLoader.hpp"

namespace Arcade {
class Core {
private:
  enum class State {
    Menu,
    Playing,
  };
  State _state;

  std::vector<std::string> _graphicalLibs;
  std::vector<std::string> _gameLibs;

  size_t _currentGraphIdx;
  size_t _currentGameIdx;
  size_t _menuSelectionIdx;

  std::unique_ptr<DLLoader<IGraphics>> _graphLoader;
  std::unique_ptr<IGraphics> _graph;

  std::unique_ptr<DLLoader<IGame>> _gameLoader;
  std::unique_ptr<IGame> _game;

  std::string _playerName;
  bool _isRunning;

  void loadGraphics(size_t index);
  void loadGame(size_t index);
  void handleGlobalInput(InputAction action);

  void runMenu();
  void runGame();

  std::vector<Cell> stringToCells(const std::string& str, float startX, float startY, std::uint8_t color, std::uint8_t textColor);

public:
  explicit Core(const std::string &initalGraphicLib, const std::string& playerName = "Player 1");
  ~Core();
  void run();
};
}

#endif /* CORE */
