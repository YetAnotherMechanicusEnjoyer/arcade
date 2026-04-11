#include "IGame.hpp"

#include <algorithm>
#include <chrono>
#include <string>
#include <vector>

namespace {
struct GridPos {
    int x;
    int y;

    bool operator==(const GridPos &other) const noexcept {
        return x == other.x && y == other.y;
    }
};

struct Ghost {
    GridPos pos{};
    GridPos start{};
    Arcade::InputAction direction{Arcade::InputAction::Left};
};

class PacmanModule : public Arcade::IGame {
public:
    PacmanModule() {
        reset();
    }

    ~PacmanModule() override = default;

    void reset() override {
        _level = 1;
        _score = 0;
        _gameOver = false;
        _playerDirection = Arcade::InputAction::Left;
        _requestedDirection = _playerDirection;
        _lastStep = std::chrono::steady_clock::now();
        startLevel();
    }

    void update() override {
        if (_gameOver)
            return;

        auto now = std::chrono::steady_clock::now();

        if (now - _lastStep < kStepDelay)
            return;
        _lastStep = now;
        movePacman();
    }

    void onInput(Arcade::InputAction action) override {
        switch (action) {
            case Arcade::InputAction::Up:
            case Arcade::InputAction::Down:
            case Arcade::InputAction::Left:
            case Arcade::InputAction::Right:
                _requestedDirection = action;
                break;
            default:
                break;
        }
    }

    std::vector<Arcade::Cell> getDisplay() const override {
        std::vector<Arcade::Cell> cells;

        appendText(cells, 0, 0, "Pacman", 4);
        appendText(cells, 0, 1, "Score: " + std::to_string(_score) + "  Level: " + std::to_string(_level), 3);
        appendText(cells, 0, 2, "Arrows: move | R: restart | M: menu", 7);

        for (int y = 0; y < kMapHeight; ++y) {
            for (int x = 0; x < kMapWidth; ++x) {
                char tile = _map[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];

                if (tile == '#')
                    cells.push_back(makeCell(x, kTopOffset + y, '#', 5));
                else if (tile == '.')
                    cells.push_back(makeCell(x, kTopOffset + y, '.', 4));
                else if (tile == 'o')
                    cells.push_back(makeCell(x, kTopOffset + y, 'o', 2));
                else
                    cells.push_back(makeCell(x, kTopOffset + y, ' ', 1));
            }
        }

        for (const Ghost &ghost : _ghosts)
            cells.push_back(makeCell(ghost.pos.x, kTopOffset + ghost.pos.y, 'G', 2));

        cells.push_back(makeCell(_pacman.x, kTopOffset + _pacman.y, 'C', 6));

        if (_gameOver)
            appendText(cells, 4, kTopOffset + kMapHeight / 2, "Game Over - press r to restart", 2);

        return cells;
    }

    int getScore() const override {
        return _score;
    }

    std::string getName() const override {
        return "Pacman";
    }

private:
    static constexpr int kMapWidth = 19;
    static constexpr int kMapHeight = 11;
    static constexpr int kTopOffset = 4;
    static constexpr auto kStepDelay = std::chrono::milliseconds(160);

    std::vector<std::string> _map;
    GridPos _pacman{};
    GridPos _pacmanStart{};
    Arcade::InputAction _playerDirection{Arcade::InputAction::Left};
    Arcade::InputAction _requestedDirection{Arcade::InputAction::Left};
    std::vector<Ghost> _ghosts;
    int _score{0};
    int _level{1};
    int _pelletsLeft{0};
    bool _gameOver{false};
    std::chrono::steady_clock::time_point _lastStep{};

    static Arcade::Cell makeCell(int x, int y, char character, int color) {
        return Arcade::Cell{static_cast<float>(x), static_cast<float>(y), character, color};
    }

    static void appendText(std::vector<Arcade::Cell> &cells, int x, int y, const std::string &text, int color) {
        for (std::size_t i = 0; i < text.size(); ++i)
            cells.push_back(makeCell(x + static_cast<int>(i), y, text[i], color));
    }

    static std::vector<std::string> baseMap() {
        return {
            "###################",
            "#o.......#.......o#",
            "#.###.##.#.##.###.#",
            "#.................#",
            "#.###.#.###.#.###.#",
            "........#.#........",
            "#.###.#.###.#.###.#",
            "#.....#.....#.....#",
            "#.###.##.#.##.###.#",
            "#o.................",
            "###################"
        };
    }

    int countPellets() const {
        int count = 0;

        for (const std::string &row : _map) {
            count += static_cast<int>(std::count(row.begin(), row.end(), '.'));
            count += static_cast<int>(std::count(row.begin(), row.end(), 'o'));
        }
        return count;
    }

    void startLevel() {
        _map = baseMap();
        _pacmanStart = {9, 7};
        _pacman = _pacmanStart;
        _playerDirection = Arcade::InputAction::Left;
        _requestedDirection = _playerDirection;
        _ghosts = {
            Ghost{{9, 5}, {9, 5}, Arcade::InputAction::Left},
            Ghost{{8, 5}, {8, 5}, Arcade::InputAction::Right},
            Ghost{{10, 5}, {10, 5}, Arcade::InputAction::Up}
        };
        _pelletsLeft = countPellets();
        _lastStep = std::chrono::steady_clock::now();
    }

    int wrapX(int x) const {
        if (x < 0)
            return kMapWidth - 1;
        if (x >= kMapWidth)
            return 0;
        return x;
    }

    bool isWalkable(const GridPos &pos) const {
        if (pos.y < 0 || pos.y >= kMapHeight || pos.x < 0 || pos.x >= kMapWidth)
            return false;
        return _map[static_cast<std::size_t>(pos.y)][static_cast<std::size_t>(pos.x)] != '#';
    }

    GridPos nextPosition(const GridPos &from, Arcade::InputAction direction) const {
        GridPos next = from;

        if (direction == Arcade::InputAction::Up)
            --next.y;
        else if (direction == Arcade::InputAction::Down)
            ++next.y;
        else if (direction == Arcade::InputAction::Left)
            --next.x;
        else if (direction == Arcade::InputAction::Right)
            ++next.x;
        next.x = wrapX(next.x);
        return next;
    }

    void consumeTile() {
        char &tile = _map[static_cast<std::size_t>(_pacman.y)][static_cast<std::size_t>(_pacman.x)];

        if (tile == '.') {
            tile = ' ';
            _score += 10;
            --_pelletsLeft;
        } else if (tile == 'o') {
            tile = ' ';
            _score += 50;
            --_pelletsLeft;
        }
    }

    void movePacman() {
        GridPos requested = nextPosition(_pacman, _requestedDirection);

        if (isWalkable(requested))
            _playerDirection = _requestedDirection;

        GridPos next = nextPosition(_pacman, _playerDirection);

        if (!isWalkable(next))
            return;
        _pacman = next;
        consumeTile();

        if (_pelletsLeft <= 0) {
            ++_level;
            startLevel();
        }
    }
};
}

extern "C" Arcade::IGame *createGame() {
    return new PacmanModule();
}
