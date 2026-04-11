#include "IGame.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <string>
#include <vector>

namespace {
using Clock = std::chrono::steady_clock;
using namespace std::chrono_literals;

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
    Clock::time_point releaseAt{};
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
        startLevel();
    }

    void update() override {
        if (_gameOver)
            return;

        auto now = Clock::now();
        auto playerDelay = levelDelay(160ms, 12ms);
        auto ghostDelay = isFrightened() ? levelDelay(300ms, 6ms) : levelDelay(220ms, 8ms);

        if (now - _lastPlayerStep >= playerDelay) {
            _lastPlayerStep = now;
            movePacman();
            resolveCollisions();
            if (_gameOver)
                return;
            if (_pelletsLeft == 0) {
                _score += 250;
                ++_level;
                startLevel();
                return;
            }
        }

        if (now - _lastGhostStep >= ghostDelay) {
            _lastGhostStep = now;
            moveGhosts();
            resolveCollisions();
        }
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
        cells.reserve(static_cast<std::size_t>(kMapWidth * kMapHeight + 200));

        appendText(cells, 0, 0, "Pacman", 4);
        appendText(cells, 0, 1, "Score: " + std::to_string(_score) + "  Level: " + std::to_string(_level), 3);
        appendText(cells, 0, 2, isFrightened() ? "Power mode active" : "Arrows move | m menu | r restart", 7);

        for (int y = 0; y < kMapHeight; ++y) {
            for (int x = 0; x < kMapWidth; ++x) {
                char tile = _map[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
                if (tile == '#')
                    cells.push_back(makeCell(x, kTopOffset + y, '#', 5));
                else if (tile == '.')
                    cells.push_back(makeCell(x, kTopOffset + y, '.', 4));
                else if (tile == 'o')
                    cells.push_back(makeCell(x, kTopOffset + y, 'o', 2));
            }
        }

        auto now = Clock::now();
        for (const Ghost &ghost : _ghosts) {
            if (now < ghost.releaseAt)
                continue;
            cells.push_back(makeCell(ghost.pos.x, kTopOffset + ghost.pos.y, isFrightened() ? 'g' : 'G', isFrightened() ? 6 : 2));
        }
        cells.push_back(makeCell(_pacman.x, kTopOffset + _pacman.y, 'C', 4));

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
    Clock::time_point _frightenedUntil{};
    Clock::time_point _lastPlayerStep{};
    Clock::time_point _lastGhostStep{};

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
            "o.......#.#.......o",
            "#.###.#.....#.###.#",
            "#.....#.. ..#.....#",
            "#.###.##.#.##.###.#",
            "#........#........#",
            "###################"
        };
    }

    std::chrono::milliseconds levelDelay(std::chrono::milliseconds base, std::chrono::milliseconds reduction) const {
        int delta = static_cast<int>(reduction.count()) * (_level - 1);
        int capped = std::max(70, static_cast<int>(base.count()) - delta);
        return std::chrono::milliseconds(capped);
    }

    void startLevel() {
        auto now = Clock::now();

        _map = baseMap();
        _pacmanStart = {9, 7};
        _pacman = _pacmanStart;
        _playerDirection = Arcade::InputAction::Left;
        _requestedDirection = _playerDirection;
        _ghosts = {
            Ghost{{8, 6}, {8, 6}, Arcade::InputAction::Left, now + 10s},
            Ghost{{9, 6}, {9, 6}, Arcade::InputAction::Left, now + 12s},
            Ghost{{10, 6}, {10, 6}, Arcade::InputAction::Right, now + 14s}
        };
        _pelletsLeft = countPellets();
        _frightenedUntil = Clock::time_point{};
        _lastPlayerStep = now;
        _lastGhostStep = now;
    }

    int countPellets() const {
        int count = 0;

        for (const std::string &row : _map) {
            count += static_cast<int>(std::count(row.begin(), row.end(), '.'));
            count += static_cast<int>(std::count(row.begin(), row.end(), 'o'));
        }
        return count;
    }

    bool isFrightened() const {
        return Clock::now() < _frightenedUntil;
    }

    char tileAt(const GridPos &pos) const {
        if (pos.y < 0 || pos.y >= kMapHeight)
            return '#';
        int wrappedX = wrapX(pos.x);
        return _map[static_cast<std::size_t>(pos.y)][static_cast<std::size_t>(wrappedX)];
    }

    bool isWall(const GridPos &pos) const {
        return tileAt(pos) == '#';
    }

    static int wrapX(int x) {
        if (x < 0)
            return kMapWidth - 1;
        if (x >= kMapWidth)
            return 0;
        return x;
    }

    GridPos stepFrom(const GridPos &start, Arcade::InputAction dir) const {
        GridPos next = start;

        switch (dir) {
            case Arcade::InputAction::Up:
                --next.y;
                break;
            case Arcade::InputAction::Down:
                ++next.y;
                break;
            case Arcade::InputAction::Left:
                --next.x;
                break;
            case Arcade::InputAction::Right:
                ++next.x;
                break;
            default:
                break;
        }
        next.x = wrapX(next.x);
        return next;
    }

    static Arcade::InputAction opposite(Arcade::InputAction dir) {
        switch (dir) {
            case Arcade::InputAction::Up:
                return Arcade::InputAction::Down;
            case Arcade::InputAction::Down:
                return Arcade::InputAction::Up;
            case Arcade::InputAction::Left:
                return Arcade::InputAction::Right;
            case Arcade::InputAction::Right:
                return Arcade::InputAction::Left;
            default:
                return Arcade::InputAction::None;
        }
    }

    bool canMove(const GridPos &from, Arcade::InputAction dir) const {
        if (dir != Arcade::InputAction::Up && dir != Arcade::InputAction::Down
            && dir != Arcade::InputAction::Left && dir != Arcade::InputAction::Right)
            return false;
        return !isWall(stepFrom(from, dir));
    }

    void consumeTile(const GridPos &pos) {
        int x = wrapX(pos.x);
        char &tile = _map[static_cast<std::size_t>(pos.y)][static_cast<std::size_t>(x)];

        if (tile == '.') {
            tile = ' ';
            _score += 10;
            --_pelletsLeft;
        } else if (tile == 'o') {
            tile = ' ';
            _score += 50;
            --_pelletsLeft;
            _frightenedUntil = Clock::now() + 10s;
        }
    }

    void movePacman() {
        if (canMove(_pacman, _requestedDirection))
            _playerDirection = _requestedDirection;
        if (!canMove(_pacman, _playerDirection))
            return;
        _pacman = stepFrom(_pacman, _playerDirection);
        consumeTile(_pacman);
    }

    std::vector<Arcade::InputAction> availableDirections(const Ghost &ghost) const {
        std::vector<Arcade::InputAction> dirs;
        constexpr std::array<Arcade::InputAction, 4> allDirs = {
            Arcade::InputAction::Up,
            Arcade::InputAction::Down,
            Arcade::InputAction::Left,
            Arcade::InputAction::Right
        };

        for (Arcade::InputAction dir : allDirs) {
            if (canMove(ghost.pos, dir))
                dirs.push_back(dir);
        }
        return dirs;
    }

    Arcade::InputAction chooseGhostDirection(const Ghost &ghost) {
        std::vector<Arcade::InputAction> dirs = availableDirections(ghost);
        if (dirs.empty())
            return Arcade::InputAction::None;

        Arcade::InputAction reverse = opposite(ghost.direction);
        if (dirs.size() > 1) {
            dirs.erase(std::remove(dirs.begin(), dirs.end(), reverse), dirs.end());
            if (dirs.empty())
                dirs.push_back(reverse);
        }

        bool frightened = isFrightened();
        Arcade::InputAction best = dirs.front();
        int bestScore = std::abs(stepFrom(ghost.pos, best).x - _pacman.x) + std::abs(stepFrom(ghost.pos, best).y - _pacman.y);

        for (Arcade::InputAction dir : dirs) {
            GridPos next = stepFrom(ghost.pos, dir);
            int distance = std::abs(next.x - _pacman.x) + std::abs(next.y - _pacman.y);
            if ((!frightened && distance < bestScore) || (frightened && distance > bestScore)) {
                best = dir;
                bestScore = distance;
            }
        }
        return best;
    }

    void moveGhosts() {
        auto now = Clock::now();

        for (Ghost &ghost : _ghosts) {
            if (now < ghost.releaseAt)
                continue;
            Arcade::InputAction chosen = chooseGhostDirection(ghost);
            if (chosen == Arcade::InputAction::None)
                continue;
            ghost.direction = chosen;
            ghost.pos = stepFrom(ghost.pos, chosen);
        }
    }

    void resolveCollisions() {
        auto now = Clock::now();

        for (Ghost &ghost : _ghosts) {
            if (now < ghost.releaseAt || !(ghost.pos == _pacman))
                continue;
            if (isFrightened()) {
                ghost.pos = ghost.start;
                ghost.direction = Arcade::InputAction::Left;
                ghost.releaseAt = now + 1500ms;
                _score += 200;
            } else {
                _gameOver = true;
                return;
            }
        }
    }
};
}

extern "C" Arcade::IGame *createGame() {
    return new PacmanModule();
}
