#ifndef MENUSELECTOR_HPP
#define MENUSELECTOR_HPP

#include <string>
#include <vector>
#include <functional>
#include <iostream>

namespace Arcade {

class MenuSelector {
public:
    struct MenuOption {
        std::string name;
        std::string path;
        std::function<void()> action;
    };

    MenuSelector();
    ~MenuSelector();

    std::string run(const std::vector<std::string>& availableLibs);

private:
    void displayMenu(const std::vector<MenuOption>& options, int selectedIdx);
    void clearScreen();
    int getchNonBlock();

    bool _running;
    int _selectedIdx;
};

} // namespace Arcade

#endif
