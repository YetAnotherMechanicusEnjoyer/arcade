/*
** EPITECH PROJECT, 2026
** IGame
** File description:
** IGame
*/

#ifndef IGRAPHICS
  #define IGRAPHICS
  #include "Common.hpp"

namespace Arcade {
class IGraphics {
public:
    virtual ~IGraphics() = default;
    virtual void init() = 0;
    virtual void shutdown() = 0;
    virtual void clear() = 0;
    virtual void draw(const std::vector<Cell>& cells) = 0;
    virtual void display() = 0;
    virtual InputAction pollEvent() = 0;
    virtual std::string getName() const = 0;
};
}
#endif
