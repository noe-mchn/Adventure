#include "PauseState.h"
#include "Game.h"
#include "RessourceManager.h"
#include "InputManager.h"
#include "EventSystem.h"
#include <iostream>

PauseState::PauseState(Game& game)
    : State(game, StateType::Pause),
    m_selectedItemIndex(0),
    m_menuItemSpacing(50.f),
    m_menuStartY(300.f)
{
    m_transparent = true;
}

void PauseState::initialize() {
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
    m_background.setFillColor(sf::Color(0, 0, 0, 150));

    m_pauseTitle.setFont(m_font);
    m_pauseTitle.setString("Paused");
    m_pauseTitle.setCharacterSize(72);
    m_pauseTitle.setFillColor(sf::Color::White);

    centerText(m_pauseTitle, m_game.getWindow().getSize().x / 2.f, 150.f);

    addMenuItem("Resume", [this]() {
        EventSystem::getInstance()->triggerEvent("ResumeGame");
        });

    addMenuItem("Settings", [this]() {
        std::cout << "Settings menu (not implemented yet)" << std::endl;
        });

    addMenuItem("Quit to Menu", [this]() {
        m_game.getStateManager().clearStates();
        m_game.getStateManager().pushState(StateType::Menu);
        });

    selectMenuItem(0);
    updateMenuItemPositions();
}

void PauseState::handleEvent(const sf::Event& event) {
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

        case sf::Keyboard::Escape:
            EventSystem::getInstance()->triggerEvent("ResumeGame");
            break;

        default:
            break;
        }
    }
}

void PauseState::update(float dt) {
}

void PauseState::render(sf::RenderWindow& window) {
    window.draw(m_background);
    window.draw(m_pauseTitle);

    for (const auto& item : m_menuItems) {
        window.draw(item.displayText);
    }
}

void PauseState::addMenuItem(const std::string& text, const std::function<void()>& action) {
    PauseMenuItem item;
    item.text = text;
    item.action = action;
    item.isSelected = false;

    item.displayText.setFont(m_font);
    item.displayText.setString(text);
    item.displayText.setCharacterSize(36);
    item.displayText.setFillColor(sf::Color(180, 180, 180));

    m_menuItems.push_back(item);
}

void PauseState::selectMenuItem(size_t index) {
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

void PauseState::activateSelectedItem() {
    if (m_selectedItemIndex < m_menuItems.size()) {
        EventSystem::getInstance()->triggerEvent("PlaySound", { {"sound", std::string("menu_confirm")} });
        m_menuItems[m_selectedItemIndex].action();
    }
}

void PauseState::updateMenuItemPositions() {
    for (size_t i = 0; i < m_menuItems.size(); ++i) {
        centerText(
            m_menuItems[i].displayText,
            m_game.getWindow().getSize().x / 2.f,
            m_menuStartY + i * m_menuItemSpacing
        );
    }
}

void PauseState::centerText(sf::Text& text, float xCenter, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    text.setPosition(xCenter, y);
}
