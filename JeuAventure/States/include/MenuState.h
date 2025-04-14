#pragma once

#include "State.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include <string>

class MenuState : public State {
public:
    struct MenuItem {
        std::string text;
        sf::Text displayText;
        std::function<void()> action;
        bool isSelected;
    };

private:
    sf::Font m_font;
    sf::Sprite m_backgroundSprite;
    sf::Text m_titleText;
    std::vector<MenuItem> m_menuItems;
    size_t m_selectedItemIndex;

    float m_menuItemSpacing;
    float m_menuStartY;

    float m_titleBobTime;
    float m_menuItemAnimTime;

    void updateMenuItemPositions();
    void centerText(sf::Text& text, float xCenter, float y);

public:
    MenuState(Game& game);
    virtual ~MenuState() = default;

    void initialize() override;
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    void addMenuItem(const std::string& text, const std::function<void()>& action);
    void selectMenuItem(size_t index);
    void activateSelectedItem();
};
