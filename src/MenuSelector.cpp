#include "MenuSelector.hpp"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>

namespace Arcade {

MenuSelector::MenuSelector() : _running(true), _selectedIdx(0) {
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

MenuSelector::~MenuSelector() {
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag |= (ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void MenuSelector::clearScreen() {
  std::cout << "\033[2J\033[H";
}

int MenuSelector::getchNonBlock() {
  int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
  int ch = getchar();
  fcntl(STDIN_FILENO, F_SETFL, flags);
  return ch;
}

void MenuSelector::displayMenu(const std::vector<MenuOption>& options, int selectedIdx) {
  clearScreen();

  std::cout << "\n";
  std::cout << "  ╔══════════════════════════════════════╗\n";
  std::cout << "  ║        ARCADE - MENU PRINCIPAL       ║\n";
  std::cout << "  ╠══════════════════════════════════════╣\n";
  std::cout << "  ║                                      ║\n";
  std::cout << "  ║  Choisissez une librairie graphique: ║\n";
  std::cout << "  ║                                      ║\n";

  for (size_t i = 0; i < options.size(); ++i) {
    if (static_cast<int>(i) == selectedIdx) {
      std::cout << "  ║  \033[7m";
    } else {
      std::cout << "  ║  ";
    }

    std::string displayName = options[i].name;
    size_t lastSlash = displayName.find_last_of('/');
    if (lastSlash != std::string::npos) {
      displayName = displayName.substr(lastSlash + 1);
    }
    size_t dotPos = displayName.find(".so");
    if (dotPos != std::string::npos) {
      displayName = displayName.substr(0, dotPos);
    }
    dotPos = displayName.find(".dylib");
    if (dotPos != std::string::npos) {
      displayName = displayName.substr(0, dotPos);
    }

    printf("  ►  %-31s", displayName.c_str());

    if (static_cast<int>(i) == selectedIdx) {
      std::cout << "\033[0m";
    }
    std::cout << "║\n";
  }

  std::cout << "  ║                                      ║\n";

  if (selectedIdx == static_cast<int>(options.size())) {
    std::cout << "  ║  \033[7m  ►  Quitter\033[0m                        ║\n";
  } else {
    std::cout << "  ║    ►  Quitter                        ║\n";
  }

  std::cout << "  ║                                      ║\n";
  std::cout << "  ╚══════════════════════════════════════╝\n";
  std::cout << "\n";
  std::cout << "  Utilisez les flèches ↑/↓ pour naviguer\n";
  std::cout << "  Appuyez sur Entrée pour sélectionner\n";
  std::cout << "  Appuyez sur 'q' pour quitter\n";

  fflush(stdout);
}

std::string MenuSelector::run(const std::vector<std::string>& availableLibs) {
  std::vector<MenuOption> options;

  for (const auto& lib : availableLibs) {
    MenuOption opt;
    opt.name = lib;
    opt.path = lib;
    options.push_back(opt);
  }

  _selectedIdx = 0;
  _running = true;

  displayMenu(options, _selectedIdx);

  while (_running) {
    int ch = getchNonBlock();

    if (ch == EOF) {
      usleep(10000);
      continue;
    }

    switch (ch) {
      case 27:
        ch = getchNonBlock();
        if (ch == 91) {
          ch = getchNonBlock();
          switch (ch) {
            case 65:
              _selectedIdx--;
              if (_selectedIdx < 0) 
                _selectedIdx = options.size();
              displayMenu(options, _selectedIdx);
              break;
            case 66:
              _selectedIdx++;
              if (_selectedIdx > static_cast<int>(options.size())) 
                _selectedIdx = 0;
              displayMenu(options, _selectedIdx);
              break;
          }
        }
        break;

      case '\n':
      case '\r':
        if (_selectedIdx == static_cast<int>(options.size())) {
          return "";
        } else if (_selectedIdx >= 0 && _selectedIdx < static_cast<int>(options.size())) {
          return options[_selectedIdx].path;
        }
        break;

      case 'q':
      case 'Q':
        return "";
        break;
    }

    usleep(10000);
  }

  return "";
}

}
