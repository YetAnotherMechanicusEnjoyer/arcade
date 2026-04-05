#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include "Errors.hpp"
#include "DLLoader.hpp"

namespace Arcade {
class DirectoryScanner {
public:
  static void scan(const std::string& path, std::vector<std::string>& gameLibs, std::vector<std::string>& graphicLibs) {
    gameLibs.clear();
    graphicLibs.clear();

    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
      return;

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
      if (entry.path().extension() == ".so") {
        std::string filepath = entry.path().string();
        try {
          DLLoader<void> inspector(filepath);

          if (inspector.hasSymbol("createGame")) {
            gameLibs.push_back(filepath);
          } else if (inspector.hasSymbol("createGraphics")) {
            graphicLibs.push_back(filepath);
          } else {
            throw ARCError("Invalid symbol");
          }
        } catch (const ARCError& e) {
          std::cerr << "Error loading " << filepath << ": " << e.what() << std::endl;
        }
      }
    }
  }
};
}
