#include "Core.hpp"
#include "Errors.hpp"
#include "MenuSelector.hpp"
#include "DirectoryScanner.hpp"
#include <iostream>

#define SUCCESS 0
#define HELP 2026
#define ERROR 84

static unsigned int handle_args(int argc, const char *argv[]) {
  if (argc > 2) {
    std::cerr << "Usage:\n\t./arcade\n\t./arcade [graphical_library.so]\n\t./arcade -h, --help" << std::endl;
    return ERROR;
  }

  if (argc == 2) {
    std::string flag(argv[1]);
    if (flag == "--help" || flag == "-h") {
      std::cout << "Usage:\n\t./arcade [graphical_library.so]\n\n"
                   "If no library is specified, a menu will let you choose.\n\n"
                   "Options:\n\t-h, --help Show this help" << std::endl;
      return HELP;
    }
  }
  return SUCCESS;
}

int main(int argc, const char *argv[]) {
  switch (handle_args(argc, argv)) {
    case HELP:
      return SUCCESS;
    case ERROR:
      return ERROR;
    default:
      try {
        std::string selectedLib;

        if (argc == 1) {
          std::vector<std::string> gameLibs;
          std::vector<std::string> graphicalLibs;
          Arcade::DirectoryScanner::scan("./lib/", gameLibs, graphicalLibs);

          if (graphicalLibs.empty()) {
            throw Arcade::ARCError("No graphical library found in ./lib/");
          }

          Arcade::MenuSelector menu;
          selectedLib = menu.run(graphicalLibs);

          if (selectedLib.empty()) { return SUCCESS; }
        } else { selectedLib = argv[1]; }

        Arcade::Core core(selectedLib);

        core.run();
        return SUCCESS;
      } catch (const Arcade::ARCError &error) {
        std::cerr << "Error: " << error.what() << std::endl;
        return ERROR;
      } catch (const std::exception& e) {
        std::cerr << "Uncaught Error: " << e.what() << std::endl;
        return ERROR;
      } catch (...) {
        std::cerr << "Uncaught Error." << std::endl;
        return ERROR;
      }
  }
}
