/*
** EPITECH PROJECT, 2026
** core
** File description:
** core
*/

#include <memory>
#include <algorithm>
#include "Core.hpp"
#include "Common.hpp"
#include "DLLoader.hpp"
#include "DirectoryScanner.hpp"

namespace Arcade {
Core::Core(const std::string& initialGraphicLib, const std::string& playerName) : _state(State::Menu), _currentGraphIdx(0), _currentGameIdx(0), _menuSelectionIdx(0), _playerName(playerName), _isRunning(true) {
  DirectoryScanner::scan("./lib/", _gameLibs, _graphicalLibs);

  auto it = std::find(_graphicalLibs.begin(), _graphicalLibs.end(), initialGraphicLib);
  if (it != _graphicalLibs.end()) {
    _currentGraphIdx = std::distance(_graphicalLibs.begin(), it);
  } else {
    _graphicalLibs.insert(_graphicalLibs.begin(), initialGraphicLib);
    _currentGraphIdx = 0;
  }

  loadGraphics(_currentGraphIdx);
}

Core::~Core() {
  if (_graph) _graph->shutdown();
}

void Core::loadGraphics(size_t index) {
  if (_graphicalLibs.empty() || index >= _graphicalLibs.size()) return;
  if (_graph) {
    _graph->shutdown();
    _graph.reset();
  }

  _graphLoader = std::make_unique<DLLoader<IGraphics>>(_graphicalLibs[index]);
  _graph = _graphLoader->getInstance("createGraphics");
  _graph->init();
}

void Core::loadGame(size_t index) {
  if (_gameLibs.empty() || index >= _gameLibs.size()) return;
  if (_game) _game.reset();

  _gameLoader = std::make_unique<DLLoader<IGame>>(_gameLibs[index]);
  _game = _gameLoader->getInstance("createGame");
  _game->reset();
}

void Core::handleGlobalInput(InputAction action) {
  switch (action) {
    case InputAction::Quit:
      _isRunning = false;
      break;
    case InputAction::Menu:
      _state = State::Menu;
      break;
    case InputAction::NextGraphics:
      _currentGraphIdx = (_currentGraphIdx + 1) % _graphicalLibs.size();
      loadGraphics(_currentGraphIdx);
      break;
    case InputAction::PrevGraphics:
      _currentGraphIdx = (_currentGraphIdx == 0) ? _graphicalLibs.size() - 1 : _currentGraphIdx - 1;
      loadGraphics(_currentGraphIdx);
      break;
    case InputAction::NextGame:
      if (!_gameLibs.empty()) {
        _currentGameIdx = (_currentGameIdx + 1) % _gameLibs.size();
        loadGame(_currentGameIdx);
      }
      break;
    case InputAction::PrevGame:
      if (!_gameLibs.empty()) {
        _currentGameIdx = (_currentGameIdx == 0) ? _gameLibs.size() - 1 : _currentGameIdx -1;
        loadGame(_currentGameIdx);
      }
      break;
    case InputAction::Restart:
      if (_game) _game->reset();
      break;
    default:
      break;
  }
}

std::vector<Cell> Core::stringToCells(const std::string& str, float startX, float startY) {
  std::vector<Cell> cells;
  for (size_t i = 0; i < str.length(); ++i) {
    cells.push_back({startX + static_cast<float>(i), startY, str[i], 0});
  }
  return cells;
}

void Core::runMenu() {
  InputAction input = _graph->pollEvent();
  handleGlobalInput(input);

  if (input == InputAction::Down) _menuSelectionIdx = (_menuSelectionIdx + 1) % _gameLibs.size();
  if (input == InputAction::Up) _menuSelectionIdx = (_menuSelectionIdx == 0) ? _gameLibs.size() - 1 : _menuSelectionIdx - 1;

  if (input == InputAction::Action && !_gameLibs.empty()) {
    _currentGameIdx = _menuSelectionIdx;
    loadGame(_currentGameIdx);
    _state = State::Playing;
  }

  _graph->clear();

  std::vector<Cell> menuRender;

  auto title = stringToCells("--- Arcade Menu ---", 10.0f, 2.0f);
  menuRender.insert(menuRender.end(), title.begin(), title.end());

  auto nameInfo = stringToCells("Player: " + _playerName, 10.0f, 4.0f);
  menuRender.insert(menuRender.end(), nameInfo.begin(), nameInfo.end());
  
  auto gamesTitle = stringToCells("Available Games:", 10.0f, 6.0f);
  menuRender.insert(menuRender.end(), gamesTitle.begin(), gamesTitle.end());

  for (size_t i = 0; i < _gameLibs.size(); ++i) {
    std::string prefix = (i == _menuSelectionIdx) ? "> " : "  ";
    auto gameName = stringToCells(prefix + _gameLibs[i], 12.0f, 8.0f + i);
    menuRender.insert(menuRender.end(), gameName.begin(), gameName.end());
  }

  _graph->draw(menuRender);
  _graph->display();
}

void Core::runGame() {
  InputAction input = _graph->pollEvent();
  handleGlobalInput(input);

  if (_state == State::Playing && _game) {
    _game->onInput(input);
    _game->update();

    _graph->clear();
    _graph->draw(_game->getDisplay());

    auto scoreDisplay = stringToCells("Score: " + std::to_string(_game->getScore()), 0.0f, 0.0f);
    _graph->draw(scoreDisplay);

    _graph->display();
  }
}

void Core::run() {
  while (_isRunning) {
    if (_state == State::Menu) {
      runMenu();
    } else if (_state == State::Playing) {
      runGame();
    }
  }
}

}
