#include "GameOverState.h"
#include "Game.h"
#include "RessourceManager.h"
#include "InputManager.h"
#include "EventSystem.h"
#include "SaveSystem.h"
#include <iostream>
#include <cmath>

GameOverState::GameOverState(Game& game)
    : State(game, StateType::GameOver),
    m_selectedItemIndex(0),
    m_menuItemSpacing(50.f),
    m_menuStartY(350.f),
    m_animTimer(0.0f)
{
    m_transparent = false;
}

void GameOverState::initialize() {
    auto* resourceManager = RessourceManager::getInstance();

    if (!resourceManager->loadFont("main_font", "Assets/Fonts/pixeloid.ttf")) {
        std::cerr << "Failed to load main font!" << std::endl;
        m_font = sf::Font();
        if (!m_font.loadFromFile("Assets/Fonts/pixeloid.ttf")) {
            std::cerr << "Failed to load fallback font!" << std::endl;
        }
    }
    else {
        m_font = *resourceManager->getFont("main_font");
    }

    m_background.setSize(sf::Vector2f(m_game.getWindow().getSize().x, m_game.getWindow().getSize().y));
    m_background.setFillColor(sf::Color(0, 0, 0, 200));

    m_gameOverText.setFont(m_font);
    m_gameOverText.setString("Game Over");
    m_gameOverText.setCharacterSize(90);
    m_gameOverText.setFillColor(sf::Color::Red);
    m_gameOverText.setOutlineColor(sf::Color::Black);
    m_gameOverText.setOutlineThickness(3.0f);

    centerText(m_gameOverText, m_game.getWindow().getSize().x / 2.f, 150.f);

    m_scoreText.setFont(m_font);
    int score = SaveSystem::getInstance()->getInt("current_score", 0);
    m_scoreText.setString("Score: " + std::to_string(score));
    m_scoreText.setCharacterSize(40);
    m_scoreText.setFillColor(sf::Color::White);

    centerText(m_scoreText, m_game.getWindow().getSize().x / 2.f, 240.f);

    addMenuItem("Try Again", [this]() {
        m_game.getStateManager().popState();
        EventSystem::getInstance()->triggerEvent("RestartLevel");
        });

    addMenuItem("Quit to Menu", [this]() {
        m_game.getStateManager().clearStates();
        m_game.getStateManager().pushState(StateType::Menu);
        });

    selectMenuItem(0);
    updateMenuItemPositions();
}

void GameOverState::handleEvent(const sf::Event& event) {
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

void GameOverState::update(float dt) {
    m_animTimer += dt;

    float scale = 1.0f + 0.1f * std::sin(m_animTimer * 2.0f);
    m_gameOverText.setScale(scale, scale);

    centerText(m_gameOverText, m_game.getWindow().getSize().x / 2.f, 150.f);
}

void GameOverState::render(sf::RenderWindow& window) {
    window.draw(m_background);
    window.draw(m_gameOverText);
    window.draw(m_scoreText);

    for (const auto& item : m_menuItems) {
        window.draw(item.displayText);
    }
}

void GameOverState::addMenuItem(const std::string& text, const std::function<void()>& action) {
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

void GameOverState::selectMenuItem(size_t index) {
    if (index >= m_menuItems.size()) return;

    if (m_selectedItemIndex < m_menuItems.size()) {
        m_menuItems[m_selectedItemIndex].isSelected = false;
        m_menuItems[m_selectedItemIndex].displayText.setFillColor(sf::Color(180, 180, 180));
        m_menuItems[m_selectedItemIndex].displayText.setOutlineThickness(0);
    }

    m_selectedItemIndex = index;
    m_menuItems[m_selectedItemIndex].isSelected = true;
    m_menuItems[m_selectedItemIndex].displayText.setFillColor(sf::Color::White);
    m_menuItems[m_selectedItemIndex].displayText.setOutlineColor(sf::Color(100, 100, 255));
    m_menuItems[m_selectedItemIndex].displayText.setOutlineThickness(2.0f);

    EventSystem::getInstance()->triggerEvent("PlaySound", { {"sound", std::string("menu_select")} });
}

void GameOverState::activateSelectedItem() {
    if (m_selectedItemIndex < m_menuItems.size()) {
        EventSystem::getInstance()->triggerEvent("PlaySound", { {"sound", std::string("menu_confirm")} });
        m_menuItems[m_selectedItemIndex].action();
    }
}

void GameOverState::updateMenuItemPositions() {
    for (size_t i = 0; i < m_menuItems.size(); ++i) {
        centerText(
            m_menuItems[i].displayText,
            m_game.getWindow().getSize().x / 2.f,
            m_menuStartY + i * m_menuItemSpacing
        );
    }
}

void GameOverState::centerText(sf::Text& text, float xCenter, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    text.setPosition(xCenter, y);
}
