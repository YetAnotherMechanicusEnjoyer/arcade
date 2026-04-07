#include "IGraphic.hpp"
#include <ncurses.h>
#include <map>

class NcursesModule : public Arcade::IGraphics {
private:
    std::map<Arcade::InputAction, int> _keyMapping;
    bool _initialized;

    void initKeyMapping() {
        _keyMapping[Arcade::InputAction::Up] = KEY_UP;
        _keyMapping[Arcade::InputAction::Down] = KEY_DOWN;
        _keyMapping[Arcade::InputAction::Left] = KEY_LEFT;
        _keyMapping[Arcade::InputAction::Right] = KEY_RIGHT;
        _keyMapping[Arcade::InputAction::Quit] = 'q';
        _keyMapping[Arcade::InputAction::Menu] = 'm';
        _keyMapping[Arcade::InputAction::Restart] = 'r';
        _keyMapping[Arcade::InputAction::NextGraphics] = 'g';
        _keyMapping[Arcade::InputAction::PrevGraphics] = 'G';
        _keyMapping[Arcade::InputAction::NextGame] = 'n';
        _keyMapping[Arcade::InputAction::PrevGame] = 'N';
        _keyMapping[Arcade::InputAction::Action] = '\n';
    }

    Arcade::InputAction convertKeyToAction(int ch) {
        for (const auto& [action, key] : _keyMapping) {
            if (key == ch) return action;
        }
        return Arcade::InputAction::None;
    }

public:
    NcursesModule() : _initialized(false) {
        initKeyMapping();
    }

    ~NcursesModule() {
        if (_initialized) shutdown();
    }

    void init() override {
        if (_initialized) return;
        
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        curs_set(0);
        start_color();
        
        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);
        init_pair(5, COLOR_BLUE, COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_CYAN, COLOR_BLACK);
        
        clear();
        _initialized = true;
    }

    void shutdown() override {
        if (!_initialized) return;
        endwin();
        _initialized = false;
    }

    void clear() override {
        ::clear();
    }

    void draw(const std::vector<Arcade::Cell>& cells) override {
        for (const auto& cell : cells) {
            int y = static_cast<int>(cell.y);
            int x = static_cast<int>(cell.x);
            
            if (x < 0 || y < 0) continue;
            
            if (cell.color > 0 && cell.color <= 7) {
                attron(COLOR_PAIR(cell.color));
            }
            
            mvaddch(y, x, cell.character);
            
            if (cell.color > 0 && cell.color <= 7) {
                attroff(COLOR_PAIR(cell.color));
            }
        }
    }

    void display() override {
        refresh();
    }

    Arcade::InputAction pollEvent() override {
        int ch = getch();
        if (ch == ERR) return Arcade::InputAction::None;
        
        return convertKeyToAction(ch);
    }

    std::string getName() const override {
        return "Ncurses";
    }
};

extern "C" Arcade::IGraphics* createGraphics() {
    return new NcursesModule();
}
