#include "IGame.hpp"

#include <deque>
#include <string>
#include <vector>

namespace {
struct GridPos {
    int x;
    int y;
};

class SnakeModule : public Arcade::IGame {
public:
    SnakeModule()
    {
        reset();
    }

    ~SnakeModule() override = default;

    void reset() override
    {
        _snake.clear();
        const int midX = kBoardWidth / 2;
        const int midY = kBoardHeight / 2;
        _snake.push_back({midX, midY});
        _snake.push_back({midX - 1, midY});
        _snake.push_back({midX - 2, midY});
        _snake.push_back({midX - 3, midY});
        _score = 0;
    }

    void update() override
    {
    }

    void onInput(Arcade::InputAction) override
    {
    }

    std::vector<Arcade::Cell> getDisplay() const override
    {
        std::vector<Arcade::Cell> cells;

        appendText(cells, 0, 0, "Snake", 4);
        appendText(cells, 0, 1, "Score: " + std::to_string(_score), 3);
        appendText(cells, 0, 2, "Skeleton version", 7);

        for (int x = 0; x < kBoardWidth; ++x) {
            cells.push_back(makeCell(x, kTopOffset, '#', 5));
            cells.push_back(makeCell(x, kTopOffset + kBoardHeight - 1, '#', 5));
        }
        for (int y = 1; y < kBoardHeight - 1; ++y) {
            cells.push_back(makeCell(0, kTopOffset + y, '#', 5));
            cells.push_back(makeCell(kBoardWidth - 1, kTopOffset + y, '#', 5));
        }

        for (std::size_t i = 0; i < _snake.size(); ++i) {
            const GridPos &part = _snake[i];
            cells.push_back(makeCell(part.x, kTopOffset + part.y, i == 0 ? '@' : 'o', i == 0 ? 2 : 3));
        }

        return cells;
    }

    int getScore() const override
    {
        return _score;
    }

    std::string getName() const override
    {
        return "Snake";
    }

private:
    static constexpr int kBoardWidth = 30;
    static constexpr int kBoardHeight = 20;
    static constexpr int kTopOffset = 4;

    std::deque<GridPos> _snake;
    int _score{0};

    static Arcade::Cell makeCell(int x, int y, char character, int color)
    {
        return Arcade::Cell{static_cast<float>(x), static_cast<float>(y), character, color};
    }

    static void appendText(std::vector<Arcade::Cell> &cells, int x, int y, const std::string &text, int color)
    {
        for (std::size_t i = 0; i < text.size(); ++i) {
            cells.push_back(makeCell(x + static_cast<int>(i), y, text[i], color));
        }
    }
};
}

extern "C" Arcade::IGame *createGame()
{
    return new SnakeModule();
}
