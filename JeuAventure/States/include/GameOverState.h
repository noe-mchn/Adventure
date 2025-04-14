#pragma once

#include "State.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include <string>

class GameOverState : public State {
private:
    struct MenuItem {
        std::string text;
        sf::Text displayText;
        std::function<void()> action;
        bool isSelected;
    };

    sf::Font m_font;
    sf::RectangleShape m_background;
    sf::Text m_gameOverText;
    sf::Text m_scoreText;
    std::vector<MenuItem> m_menuItems;
    size_t m_selectedItemIndex;

    float m_menuItemSpacing;
    float m_menuStartY;

    float m_animTimer;

    void updateMenuItemPositions();
    void centerText(sf::Text& text, float xCenter, float y);

public:
    GameOverState(Game& game);
    virtual ~GameOverState() = default;

    void initialize() override;
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    void addMenuItem(const std::string& text, const std::function<void()>& action);
    void selectMenuItem(size_t index);
    void activateSelectedItem();
};
