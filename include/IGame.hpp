/*
** EPITECH PROJECT, 2026
** IGame
** File description:
** IGame
*/

#ifndef IGAME
  #define IGAME
  #include "Common.hpp"
 
namespace Arcade {
class IGame {
public:
  virtual ~IGame() = default;
  virtual void reset() = 0;
  virtual void update() = 0;
  virtual void onInput(InputAction action) = 0;
  virtual std::vector<Cell> getDisplay() const = 0;
  virtual int getScore() const = 0;
  virtual std::string getName() const = 0;
};
}
#endif
