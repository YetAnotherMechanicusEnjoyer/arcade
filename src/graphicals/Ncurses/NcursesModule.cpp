#include "IGraphic.hpp"

class NcursesModule : public Arcade::IGraphics {
public:
  NcursesModule() = default;
  ~NcursesModule() = default;
  void init() override {
    return;
  };
  void shutdown() override {
    return;
  };
  void clear() override {
    return;
  };
  void draw(const std::vector<Arcade::Cell>& cells) override {
    return;
  };
  void display() override {
    return;
  };
  Arcade::InputAction pollEvent() override {
    return Arcade::InputAction::Exit;
  };
  std::string getName() const override {
    return std::string("todo");
  };
};

extern "C" Arcade::IGraphics* createGame() {
  return new NcursesModule();
}
