#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include "DLLoader.hpp"
#include "Errors.hpp"

namespace Arcade {
class DirectoryScanner {
public:
  static void scan(const std::string& path, std::vector<std::string>& gameLibs, std::vector<std::string>& graphicLibs) {
    gameLibs.clear();
    graphicLibs.clear();

    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
      return;

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
      std::string extension = entry.path().extension().string();

      if (extension == ".so" || extension == ".dylib") {
        std::string filepath = entry.path().string();

        try {
          DLLoader<int> inspector(filepath);

          if (inspector.hasSymbol("createGame")) {
            gameLibs.push_back(filepath);
          } else if (inspector.hasSymbol("createGraphics")) {
            graphicLibs.push_back(filepath);
          } else {
            throw ARCError("Invalid symbol.");
          }
        } catch (const ARCError& e) {
          std::cerr << "Error loading " << filepath << ": " << e.what() << std::endl;
        }
      }
    }
  }
};
}
