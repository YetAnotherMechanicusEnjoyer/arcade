#include "IGame.hpp"

#include <chrono>
#include <cstdint>
#include <deque>
#include <random>
#include <string>
#include <vector>

namespace {
struct GridPos {
    int x;
    int y;
};

enum class Direction {
    Up,
    Down,
    Left,
    Right
};

class SnakeModule : public Arcade::IGame {
public:
    SnakeModule()
        : _rng(std::random_device{}())
    {
        reset();
    }

    ~SnakeModule() override = default;

    void reset() override {
        _snake.clear();
        const int midX = kBoardWidth / 2;
        const int midY = kBoardHeight / 2;
        _snake.push_back({midX, midY});
        _snake.push_back({midX - 1, midY});
        _snake.push_back({midX - 2, midY});
        _snake.push_back({midX - 3, midY});
        _score = 0;
        _gameOver = false;
        _direction = Direction::Right;
        _pendingDirection = Direction::Right;
        _lastStep = std::chrono::steady_clock::now();
        spawnFood();
    }

    void update() override {
        if (_gameOver)
            return;
        const auto now = std::chrono::steady_clock::now();
        if (now - _lastStep < kStepDelay)
            return;
        _lastStep = now;
        _direction = _pendingDirection;
        advance();
    }

    void onInput(Arcade::InputAction action) override {
        switch (action) {
            case Arcade::InputAction::Up:
                if (_direction != Direction::Down)
                    _pendingDirection = Direction::Up;
                break;
            case Arcade::InputAction::Down:
                if (_direction != Direction::Up)
                    _pendingDirection = Direction::Down;
                break;
            case Arcade::InputAction::Left:
                if (_direction != Direction::Right)
                    _pendingDirection = Direction::Left;
                break;
            case Arcade::InputAction::Right:
                if (_direction != Direction::Left)
                    _pendingDirection = Direction::Right;
                break;
            default:
                break;
        }
    }

    std::vector<Arcade::Cell> getDisplay() const override
    {
        std::vector<Arcade::Cell> cells;

        appendText(cells, 0, 0, "Snake", 0, 4);
        appendText(cells, 0, 1, "Score: " + std::to_string(_score), 0, 3);
        appendText(cells, 0, 2, "Length: " + std::to_string(_snake.size()), 0, 6);
        appendText(cells, 0, 3, _gameOver ? "Game Over - press R to restart" : "Arrows: move | R: restart | M: menu", 0, 7);

        for (int x = 0; x < kBoardWidth; ++x) {
            cells.push_back(makeCell(x, kTopOffset, '#', 0, 5));
            cells.push_back(makeCell(x, kTopOffset + kBoardHeight - 1, '#', 0, 5));
        }
        for (int y = 1; y < kBoardHeight - 1; ++y) {
            cells.push_back(makeCell(0, kTopOffset + y, '#', 0, 5));
            cells.push_back(makeCell(kBoardWidth - 1, kTopOffset + y, '#', 0, 5));
        }

        for (int y = 1; y < kBoardHeight - 1; ++y) {
            for (int x = 1; x < kBoardWidth - 1; ++x) {
                cells.push_back(makeCell(x, kTopOffset + y, ' ', 0, 1));
            }
        }

        cells.push_back(makeCell(_food.x, kTopOffset + _food.y, '$', 0, 4));

        for (std::size_t i = 0; i < _snake.size(); ++i) {
            const GridPos &part = _snake[i];
            cells.push_back(makeCell(part.x, kTopOffset + part.y, i == 0 ? 'O' : 'o', 0, i == 0 ? 2 : 3));
        }

        if (_gameOver) {
            appendText(cells, 6, kTopOffset + kBoardHeight / 2, "GAME OVER", 0, 4);
        }

        return cells;
    }

    int getScore() const override {
        return _score;
    }

    std::string getName() const override {
        return "Snake";
    }

private:
    static constexpr int kBoardWidth = 30;
    static constexpr int kBoardHeight = 20;
    static constexpr int kTopOffset = 4;
    static constexpr auto kStepDelay = std::chrono::milliseconds(140);

    std::deque<GridPos> _snake;
    GridPos _food{};
    int _score{0};
    bool _gameOver{false};
    Direction _direction{Direction::Right};
    Direction _pendingDirection{Direction::Right};
    std::chrono::steady_clock::time_point _lastStep{};
    mutable std::mt19937 _rng;

    static Arcade::Cell makeCell(int x, int y, char character, std::uint8_t color, std::uint8_t textColor) {
        return Arcade::Cell{static_cast<float>(x), static_cast<float>(y), character, color, textColor};
    }

    static void appendText(std::vector<Arcade::Cell> &cells, int x, int y, const std::string &text, std::uint8_t color, std::uint8_t text_color) {
        for (std::size_t i = 0; i < text.size(); ++i) {
            cells.push_back(makeCell(x + static_cast<int>(i), y, text[i], color, text_color));
        }
    }

    bool isOnSnake(const GridPos &pos) const {
        for (const GridPos &part : _snake) {
            if (part.x == pos.x && part.y == pos.y)
                return true;
        }
        return false;
    }

    void spawnFood() {
        std::uniform_int_distribution<int> distX(1, kBoardWidth - 2);
        std::uniform_int_distribution<int> distY(1, kBoardHeight - 2);

        GridPos candidate{};
        do {
            candidate = {distX(_rng), distY(_rng)};
        } while (isOnSnake(candidate));

        _food = candidate;
    }

    bool isBodyCollision(const GridPos &pos) const {
        for (const GridPos &part : _snake) {
            if (part.x == pos.x && part.y == pos.y)
                return true;
        }
        return false;
    }

    void advance() {
        if (_snake.empty())
            return;
        GridPos next = _snake.front();
        switch (_direction) {
            case Direction::Up:
                --next.y;
                break;
            case Direction::Down:
                ++next.y;
                break;
            case Direction::Left:
                --next.x;
                break;
            case Direction::Right:
                ++next.x;
                break;
        }

        const int minX = 1;
        const int maxX = kBoardWidth - 2;
        const int minY = 1;
        const int maxY = kBoardHeight - 2;

        if (next.x < minX || next.x > maxX || next.y < minY || next.y > maxY) {
            _gameOver = true;
            return;
        }

        bool grows = (next.x == _food.x && next.y == _food.y);

        if (!grows && !_snake.empty()) {
            GridPos tail = _snake.back();
            _snake.pop_back();
            if (isBodyCollision(next)) {
                _snake.push_back(tail);
                _gameOver = true;
                return;
            }
            _snake.push_back(tail);
        } else if (isBodyCollision(next)) {
            _gameOver = true;
            return;
        }

        _snake.push_front(next);
        if (grows) {
            _score += 10;
            spawnFood();
        } else {
            _snake.pop_back();
        }
    }
};
}

extern "C" Arcade::IGame *createGame()
{
    return new SnakeModule();
}
