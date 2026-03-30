/*
** EPITECH PROJECT, 2026
** arcade
** File description:
** arcade
*/

#include "Core.hpp"
#include "Errors.hpp"
#include <cstdlib>
#include <iostream>

static unsigned int handle_args(int argc, const char *argv[]) {
  if (argc != 2 || !argv || !argv[1]) {
    std::cerr <<
      "Usage:\n\tarcade [options] <file>\nOptions:\n\t-h, --help Show this help" <<
      std::endl;
    return ERROR;
  }

  std::string flag(argv[1]);
  if (flag == "--help" || flag == "-h") {
    std::cout <<
      "Usage:\n\tarcade [options] <file>\nOptions:\n\t-h, --help Show this help" <<
      std::endl;
    return HELP;
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
        Arcade::Core core(argv[1]);

        core.run();
        return EXIT_SUCCESS;
      } catch (const Arcade::ARCError &error) {
        std::cerr << "Error: " << error.what() << std::endl;
        return EXIT_FAILURE;
      } catch (...) {
        std::cerr << "Uncaught Error." << std::endl;
        return EXIT_FAILURE;
      }
    }
}
