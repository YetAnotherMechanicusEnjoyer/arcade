#include "IGame.hpp"

class SnakeModule : public Arcade::IGame {
public:
  SnakeModule() = default;
  ~SnakeModule() = default;
  void reset() override {
    return;
  };
  void update() override {
    return;
  };
  void onInput(Arcade::InputAction) override {
    return;
  };
  std::vector<Arcade::Cell> getDisplay() const override {
    return std::vector<Arcade::Cell>();
  };
  int getScore() const override {
    return 0;
  };
  std::string getName() const override {
    return std::string("Snake");
  };
};

extern "C" Arcade::IGame* createGame() {
  return new SnakeModule();
}
