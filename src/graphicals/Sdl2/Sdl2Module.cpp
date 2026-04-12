#include "IGraphic.hpp"
#include <SDL2/SDL_video.h>
#include <cstdint>
#include <vector>

#ifdef __APPLE__
  #include <SDL.h>
  #include <SDL_ttf.h>
  #define FONT "/System/Library/Fonts/Monaco.ttf"
#else
  #include <SDL2/SDL.h>
  #include <SDL2/SDL_ttf.h>
  #define FONT "/usr/share/fonts/TTF/JetBrainsMonoNerdFontMono-Medium.ttf"
#endif

#include <map>
#include <iostream>

#define COLOR_WHITE {200, 200, 200}
#define COLOR_RED {200, 0, 0}
#define COLOR_GREEN {0, 200, 0}
#define COLOR_YELLOW {200, 200, 0}
#define COLOR_BLUE {0, 0, 200}
#define COLOR_MAGENTA {200, 0, 200}
#define COLOR_CYAN {35, 200, 200}

static std::vector<Uint8> to_rgb(std::uint8_t color) {
  std::vector<Uint8> rgb;

  switch (color) {
    case 1:
      rgb = COLOR_WHITE;
      break;
    case 2:
      rgb = COLOR_RED;
      break;
    case 3:
      rgb = COLOR_GREEN;
      break;
    case 4:
      rgb = COLOR_YELLOW;
      break;
    case 5:
      rgb = COLOR_BLUE;
      break;
    case 6:
      rgb = COLOR_MAGENTA;
      break;
    case 7:
      rgb = COLOR_CYAN;
      break;
    default:
      rgb = {20, 20, 20};
  }
  return rgb;
}

class SDL2Module : public Arcade::IGraphics {
private:
    SDL_Window* _window;
    SDL_Renderer* _renderer;
    TTF_Font* _font;
    std::map<Arcade::InputAction, SDL_Keycode> _keyMapping;
    bool _initialized;
    int _cellSize;

    void initKeyMapping() {
        _keyMapping[Arcade::InputAction::Up] = SDLK_UP;
        _keyMapping[Arcade::InputAction::Down] = SDLK_DOWN;
        _keyMapping[Arcade::InputAction::Left] = SDLK_LEFT;
        _keyMapping[Arcade::InputAction::Right] = SDLK_RIGHT;
        _keyMapping[Arcade::InputAction::Quit] = SDLK_q;
        _keyMapping[Arcade::InputAction::Menu] = SDLK_m;
        _keyMapping[Arcade::InputAction::Restart] = SDLK_r;
        _keyMapping[Arcade::InputAction::NextGraphics] = SDLK_g;
        _keyMapping[Arcade::InputAction::PrevGraphics] = 103;
        _keyMapping[Arcade::InputAction::NextGame] = SDLK_n;
        _keyMapping[Arcade::InputAction::PrevGame] = 110;
        _keyMapping[Arcade::InputAction::Action] = SDLK_RETURN;
    }

    Arcade::InputAction convertKeyToAction(SDL_Keycode key) {
        for (const auto& [action, sdlKey] : _keyMapping) {
            if (sdlKey == key) return action;
        }
        return Arcade::InputAction::None;
    }

public:
    SDL2Module() : _window(nullptr), _renderer(nullptr), _font(nullptr), _initialized(false), _cellSize(30) {
        initKeyMapping();
    }

    ~SDL2Module() {
        if (_initialized) shutdown();
    }

    void init() override {
        if (_initialized) return;

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL2 Init failed: " << SDL_GetError() << std::endl;
            return;
        }

        if (TTF_Init() < 0) {
            std::cerr << "SDL_ttf Init failed: " << TTF_GetError() << std::endl;
            SDL_Quit();
            return;
        }

        _window = SDL_CreateWindow(
            "Arcade - SDL2",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1280, 720,
            SDL_WINDOW_RESIZABLE
        );

        if (!_window) {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            TTF_Quit();
            SDL_Quit();
            return;
        }

        _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
        if (!_renderer) {
            std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(_window);
            TTF_Quit();
            SDL_Quit();
            return;
        }

        _font = TTF_OpenFont(FONT, 20);
        if (!_font) {
            std::cerr << "Warning: Could not load font: " << TTF_GetError() << std::endl;
        }

        _initialized = true;
    }

    void shutdown() override {
        if (!_initialized) return;

        if (_font) TTF_CloseFont(_font);
        if (_renderer) SDL_DestroyRenderer(_renderer);
        if (_window) SDL_DestroyWindow(_window);
        TTF_Quit();
        SDL_Quit();
        _initialized = false;
    }

    void clear() override {
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
        SDL_RenderClear(_renderer);
    }

    void draw(const std::vector<Arcade::Cell>& cells) override {
        for (const auto& cell : cells) {
            int x = static_cast<int>(cell.x) * _cellSize;
            int y = static_cast<int>(cell.y) * _cellSize;

            SDL_Rect rect = {x, y, _cellSize, _cellSize};

            std::vector<Uint8> color = to_rgb(cell.color);
            std::vector<Uint8> textVecColor = to_rgb(cell.textColor);

            SDL_SetRenderDrawColor(_renderer, color[0], color[1], color[2], 255);
            SDL_RenderFillRect(_renderer, &rect);

            if (_font && cell.character != ' ') {
                char text[2] = {cell.character, '\0'};
                SDL_Color textColor = {textVecColor[0], textVecColor[1], textVecColor[2], 255};

                SDL_Surface* surface = TTF_RenderText_Blended(_font, text, textColor);
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
                    if (texture) {
                        int textX = x + (_cellSize - surface->w) / 2;
                        int textY = y + (_cellSize - surface->h) / 2;
                        SDL_Rect textRect = {textX, textY, surface->w, surface->h};
                        SDL_RenderCopy(_renderer, texture, nullptr, &textRect);
                        SDL_DestroyTexture(texture);
                    }
                    SDL_FreeSurface(surface);
                }
            }
        }
    }

    void display() override {
        SDL_RenderPresent(_renderer);
    }

    Arcade::InputAction pollEvent() override {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return Arcade::InputAction::Quit;
            }
            if (event.type == SDL_KEYDOWN) {
                return convertKeyToAction(event.key.keysym.sym);
            }
        }
        return Arcade::InputAction::None;
    }

    std::string getName() const override {
        return "SDL2";
    }
};

extern "C" Arcade::IGraphics* createGraphics() {
    return new SDL2Module();
}
