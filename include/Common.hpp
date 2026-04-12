/*
** EPITECH PROJECT, 2026
** Common
** File description:
** Common
*/

#ifndef COMMON
  #define COMMON
  #include <cstddef>
#include <cstdint>
  #include <string>
  #include <vector>

namespace Arcade {
  enum class PluginType {
    Game,
    Graphics
  };

  enum class InputAction {
    None,
    Quit,
    NextGraphics,
    PrevGraphics,
    NextGame,
    PrevGame,
    Restart,
    Menu,
    Select,
    Backspace,
    Up,
    Down,
    Left,
    Right,
    Action
  };

  struct KeyEvent {
    InputAction action {InputAction::None};
    char text {'\0'};
  };

  enum class TileType {
    Empty,
    Wall,
    Player,
    Enemy,
    Food,
    Bonus,
    Text
  };

  struct Vec2i {
    int x {0};
    int y {0};
  };

  enum class CellType {
    Empty,
    Wall,
    Player,
    Enemy,
    Food,
    Text
  };

  struct Cell {
    float x;
    float y;
    char character;
    std::uint8_t color;
    std::uint8_t textColor;
  };

  struct Drawable {
    TileType type {TileType::Empty};
    Vec2i pos {};
    std::string text {};
  };

  struct GameState {
    std::string name {"Unknown game"};
    std::vector<Drawable> drawables {};
    int score {0};
    bool finished {false};
  };

  struct MenuState {
    std::vector<std::string> graphics {};
    std::vector<std::string> games {};
    std::string playerName {"player"};
    std::size_t selectedGraphics {0};
    std::size_t selectedGame {0};
  };
}
#endif
