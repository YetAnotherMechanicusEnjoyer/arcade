#include "IGraphic.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cstdint>

static sf::Color toSFMLColor(std::uint8_t colorIndex) {
    switch (colorIndex) {
        case 1: return sf::Color::White;
        case 2: return sf::Color::Red;
        case 3: return sf::Color::Green;
        case 4: return sf::Color::Yellow;
        case 5: return sf::Color::Blue;
        case 6: return sf::Color::Magenta;
        case 7: return sf::Color::Cyan;
        default: return sf::Color(20, 20, 20);
    }
}

class SFMLModule : public Arcade::IGraphics {
private:
    sf::RenderWindow* _window;
    sf::Font _font;
    std::map<Arcade::InputAction, sf::Keyboard::Key> _keyMapping;
    bool _initialized;
    int _cellSize;
    float _windowWidth;
    float _windowHeight;

    void initKeyMapping() {
        _keyMapping[Arcade::InputAction::Up] = sf::Keyboard::Up;
        _keyMapping[Arcade::InputAction::Down] = sf::Keyboard::Down;
        _keyMapping[Arcade::InputAction::Left] = sf::Keyboard::Left;
        _keyMapping[Arcade::InputAction::Right] = sf::Keyboard::Right;
        _keyMapping[Arcade::InputAction::Quit] = sf::Keyboard::Q;
        _keyMapping[Arcade::InputAction::Menu] = sf::Keyboard::M;
        _keyMapping[Arcade::InputAction::Restart] = sf::Keyboard::R;
        _keyMapping[Arcade::InputAction::NextGraphics] = sf::Keyboard::G;
        _keyMapping[Arcade::InputAction::PrevGraphics] = sf::Keyboard::H;
        _keyMapping[Arcade::InputAction::NextGame] = sf::Keyboard::N;
        _keyMapping[Arcade::InputAction::PrevGame] = sf::Keyboard::B;
        _keyMapping[Arcade::InputAction::Action] = sf::Keyboard::Return;
    }

    Arcade::InputAction convertKeyToAction(sf::Keyboard::Key key) {
        for (const auto& [action, mappedKey] : _keyMapping) {
            if (mappedKey == key) return action;
        }
        return Arcade::InputAction::None;
    }

public:
    SFMLModule() : _window(nullptr), _initialized(false), _cellSize(25), _windowWidth(800), _windowHeight(600) {
        initKeyMapping();
    }

    ~SFMLModule() {
        if (_initialized) shutdown();
    }

    void init() override {
        if (_initialized) return;

        _window = new sf::RenderWindow(sf::VideoMode(1280, 720), "Arcade - SFML", 
                                       sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
        
        if (!_window) {
            std::cerr << "SFML Window creation failed" << std::endl;
            return;
        }

        _window->setFramerateLimit(60);
        
        #ifdef __APPLE__
        if (!_font.loadFromFile("/System/Library/Fonts/Monaco.ttf")) {
            std::cerr << "Warning: Could not load font" << std::endl;
        }
        #else
        if (!_font.loadFromFile("/usr/share/fonts/TTF/DejaVuSansMono.ttf")) {
            std::cerr << "Warning: Could not load font" << std::endl;
        }
        #endif

        _initialized = true;
    }

    void shutdown() override {
        if (!_initialized) return;

        if (_window) {
            _window->close();
            delete _window;
            _window = nullptr;
        }
        _initialized = false;
    }

    void clear() override {
        if (_window) {
            _window->clear(sf::Color::Black);
        }
    }

    void draw(const std::vector<Arcade::Cell>& cells) override {
        if (!_window) return;

        sf::Vector2u windowSize = _window->getSize();
        float scaleX = windowSize.x / _windowWidth;
        float scaleY = windowSize.y / _windowHeight;

        for (const auto& cell : cells) {
            float x = cell.x * _cellSize * scaleX;
            float y = cell.y * _cellSize * scaleY;
            float width = _cellSize * scaleX;
            float height = _cellSize * scaleY;

            sf::RectangleShape rect(sf::Vector2f(width - 1, height - 1));
            rect.setPosition(x, y);
            rect.setFillColor(toSFMLColor(cell.color));
            
            if (cell.color == 0) {
                rect.setOutlineThickness(1);
                rect.setOutlineColor(sf::Color(40, 40, 40));
            }

            _window->draw(rect);

            if (cell.character != ' ') {
                sf::Text text;
                text.setFont(_font);
                text.setString(cell.character);
                text.setCharacterSize(static_cast<unsigned int>(_cellSize * 0.7f * scaleY));
                text.setFillColor(toSFMLColor(cell.textColor));
                
                sf::FloatRect textBounds = text.getLocalBounds();
                text.setPosition(
                    x + (width - textBounds.width) / 2.0f,
                    y + (height - textBounds.height) / 2.0f - textBounds.top
                );
                
                _window->draw(text);
            }
        }
    }

    void display() override {
        if (_window) {
            _window->display();
        }
    }

    Arcade::InputAction pollEvent() override {
        if (!_window) return Arcade::InputAction::None;

        sf::Event event;
        while (_window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return Arcade::InputAction::Quit;
            }
            if (event.type == sf::Event::KeyPressed) {
                return convertKeyToAction(event.key.code);
            }
            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                _window->setView(sf::View(visibleArea));
            }
        }
        return Arcade::InputAction::None;
    }

    std::string getName() const override {
        return "SFML";
    }
};

extern "C" Arcade::IGraphics* createGraphics() {
    return new SFMLModule();
}
