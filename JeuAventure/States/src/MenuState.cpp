#include "MenuState.h"
#include "Game.h"
#include "RessourceManager.h"
#include "InputManager.h"
#include "EventSystem.h"
#include <iostream>
#include <cmath>

MenuState::MenuState(Game& game)
    : State(game, StateType::Menu),
    m_selectedItemIndex(0),
    m_menuItemSpacing(60.f),
    m_menuStartY(300.f),
    m_titleBobTime(0.f),
    m_menuItemAnimTime(0.f)
{
}

void MenuState::initialize() {
    auto* resourceManager = RessourceManager::getInstance();

    if (!resourceManager->loadFont("main_font", "arial.ttf")) {
        std::cerr << "Failed to load main font!" << std::endl;
        m_font = sf::Font();
        if (!m_font.loadFromFile("arial.ttf")) {
            std::cerr << "Failed to load fallback font!" << std::endl;
        }
    }
    else {
        m_font = *resourceManager->getFont("main_font");
    }

    if (resourceManager->loadTexture("menu_background", "Menu.png")) {
        m_backgroundSprite.setTexture(*resourceManager->getTexture("menu_background"));

        float scaleX = static_cast<float>(m_game.getWindow().getSize().x) / m_backgroundSprite.getTexture()->getSize().x;
        float scaleY = static_cast<float>(m_game.getWindow().getSize().y) / m_backgroundSprite.getTexture()->getSize().y;
        m_backgroundSprite.setScale(scaleX, scaleY);
    }

    m_titleText.setFont(m_font);
    m_titleText.setString("Platformer Adventure");
    m_titleText.setCharacterSize(72);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setOutlineColor(sf::Color(100, 100, 255));
    m_titleText.setOutlineThickness(3.0f);

    centerText(m_titleText, m_game.getWindow().getSize().x / 2.f, 150.f);

    addMenuItem("Start Game", [this]() {
        m_game.getStateManager().pushState(StateType::Game);
        });

    addMenuItem("Settings", [this]() {
        std::cout << "Settings menu (not implemented yet)" << std::endl;
        });

    addMenuItem("Exit", [this]() {
        EventSystem::getInstance()->triggerEvent("QuitGame");
        });

    selectMenuItem(0);
    updateMenuItemPositions();
}

void MenuState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        case sf::Keyboard::Up:
            if (m_selectedItemIndex > 0) {
                selectMenuItem(m_selectedItemIndex - 1);
            }
            else {
                selectMenuItem(m_menuItems.size() - 1);
            }
            break;

        case sf::Keyboard::Down:
            if (m_selectedItemIndex < m_menuItems.size() - 1) {
                selectMenuItem(m_selectedItemIndex + 1);
            }
            else {
                selectMenuItem(0);
            }
            break;

        case sf::Keyboard::Return:
        case sf::Keyboard::Space:
            activateSelectedItem();
            break;

        default:
            break;
        }
    }
}

void MenuState::update(float dt) {
    m_titleBobTime += dt;
    m_menuItemAnimTime += dt;

    float titleBobOffset = std::sin(m_titleBobTime * 2.0f) * 10.0f;
    m_titleText.setPosition(
        m_titleText.getPosition().x,
        150.f + titleBobOffset
    );

    for (size_t i = 0; i < m_menuItems.size(); ++i) {
        if (m_menuItems[i].isSelected) {
            float scale = 1.0f + std::sin(m_menuItemAnimTime * 5.0f) * 0.1f;
            m_menuItems[i].displayText.setScale(scale, scale);

            centerText(
                m_menuItems[i].displayText,
                m_game.getWindow().getSize().x / 2.f,
                m_menuStartY + i * m_menuItemSpacing
            );
        }
    }
}

void MenuState::render(sf::RenderWindow& window) {
    if (m_backgroundSprite.getTexture() != nullptr) {
        window.draw(m_backgroundSprite);
    }
    else {
        sf::RectangleShape background(sf::Vector2f(window.getSize().x, window.getSize().y));
        background.setFillColor(sf::Color(20, 20, 70));
        window.draw(background);
    }

    window.draw(m_titleText);

    for (const auto& item : m_menuItems) {
        window.draw(item.displayText);
    }
}

void MenuState::addMenuItem(const std::string& text, const std::function<void()>& action) {
    MenuItem item;
    item.text = text;
    item.action = action;
    item.isSelected = false;

    item.displayText.setFont(m_font);
    item.displayText.setString(text);
    item.displayText.setCharacterSize(36);
    item.displayText.setFillColor(sf::Color(180, 180, 180));

    m_menuItems.push_back(item);
}

void MenuState::selectMenuItem(size_t index) {
    if (index >= m_menuItems.size()) return;

    if (m_selectedItemIndex < m_menuItems.size()) {
        m_menuItems[m_selectedItemIndex].isSelected = false;
        m_menuItems[m_selectedItemIndex].displayText.setFillColor(sf::Color(180, 180, 180));
        m_menuItems[m_selectedItemIndex].displayText.setOutlineThickness(0);
        m_menuItems[m_selectedItemIndex].displayText.setScale(1.0f, 1.0f);
    }

    m_selectedItemIndex = index;
    m_menuItems[m_selectedItemIndex].isSelected = true;
    m_menuItems[m_selectedItemIndex].displayText.setFillColor(sf::Color::White);
    m_menuItems[m_selectedItemIndex].displayText.setOutlineColor(sf::Color(100, 100, 255));
    m_menuItems[m_selectedItemIndex].displayText.setOutlineThickness(2.0f);

    m_menuItemAnimTime = 0.0f;

    EventSystem::getInstance()->triggerEvent("PlaySound", { {"sound", std::string("menu_select")} });
}

void MenuState::activateSelectedItem() {
    if (m_selectedItemIndex < m_menuItems.size()) {
        EventSystem::getInstance()->triggerEvent("PlaySound", { {"sound", std::string("menu_confirm")} });
        m_menuItems[m_selectedItemIndex].action();
    }
}

void MenuState::updateMenuItemPositions() {
    for (size_t i = 0; i < m_menuItems.size(); ++i) {
        centerText(
            m_menuItems[i].displayText,
            m_game.getWindow().getSize().x / 2.f,
            m_menuStartY + i * m_menuItemSpacing
        );
    }
}

void MenuState::centerText(sf::Text& text, float xCenter, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    text.setPosition(xCenter, y);
}
