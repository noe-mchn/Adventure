#include "GameState.h"
#include "Game.h"
#include "RessourceManager.h"
#include "InputManager.h"
#include "EventSystem.h"
#include "Player.h"
#include "Level.h"
#include "UIManager.h"
#include <iostream>
#include <sstream>
#include <filesystem>

GameState::GameState(Game& game)
    : State(game, StateType::Game),
    m_showDebugInfo(false),
    m_score(0),
    m_gameOver(false),
    m_isPaused(false)

{
    std::cout << "GameState created" << std::endl;
}

GameState::~GameState() {
    std::cout << "GameState destroyed" << std::endl;
}

void GameState::initialize() {
    std::cout << "GameState::initialize - Starting game initialization..." << std::endl;

    auto* resourceManager = RessourceManager::getInstance();
    if (!resourceManager->loadFont("debug_font", "arial.ttf")) {
        std::cerr << "Failed to load debug font!" << std::endl;
        m_debugFont = sf::Font();
        if (!m_debugFont.loadFromFile("arial.ttf")) {
            std::cerr << "Failed to load fallback font!" << std::endl;
        }
    }
    else {
        m_debugFont = *resourceManager->getFont("debug_font");
    }

    m_player = std::make_unique<Player>();

    InputManager::getInstance()->bindKey(InputAction::JUMP, sf::Keyboard::Space);
    InputManager::getInstance()->bindKey(InputAction::MOVE_LEFT, sf::Keyboard::A);
    InputManager::getInstance()->bindKey(InputAction::MOVE_LEFT, sf::Keyboard::Left);
    InputManager::getInstance()->bindKey(InputAction::MOVE_RIGHT, sf::Keyboard::D);
    InputManager::getInstance()->bindKey(InputAction::MOVE_RIGHT, sf::Keyboard::Right);

    m_level = std::make_unique<Level>("MainLevel");

    std::string levelPath = "Ressources";
    std::cout << "Loading level from: " << levelPath << std::endl;

    if (!m_level->loadFromFile(levelPath)) {
        std::cerr << "Failed to load level from: " << levelPath << std::endl;
    }

    m_level->setPlayer(m_player.get());

    std::cout << "GameState initialization complete" << std::endl;
}

void GameState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::F3) {
            m_showDebugInfo = !m_showDebugInfo;
            std::cout << "Debug info " << (m_showDebugInfo ? "enabled" : "disabled") << std::endl;
        }
        else if (event.key.code == sf::Keyboard::Escape) {
            pauseGame();
        }
        else if (event.key.code == sf::Keyboard::R) {
            resetLevel();
        }
    }

    if (m_level) {
        m_level->handleEvent(event);
    }
}

void GameState::update(float dt) {
    if (m_gameOver || m_isPaused) return;

    if (m_player) {
        m_player->update(dt);

        if (m_player->getPosition().y > m_level->getHeight() + 100) {
            m_player->kill();
            return;
        }
    }

    if (m_level) {
        m_level->update(dt);
    }

    updateCamera();
    updateHUD();

    if (m_showDebugInfo) {
        std::ostringstream debugInfo;
        debugInfo << "FPS: " << static_cast<int>(1.0f / dt) << "\n";

        if (m_player) {
            sf::Vector2f playerPos = m_player->getPosition();
            debugInfo << "Player Pos: " << playerPos.x << ", " << playerPos.y << "\n";
            debugInfo << "Velocity: " << m_player->getVelocity().x << ", " << m_player->getVelocity().y << "\n";
        }

        if (m_level) {
            debugInfo << "Level: " << m_level->getName() << "\n";
            debugInfo << "Entities: " << m_level->getEntitiesInArea(sf::FloatRect(0, 0, m_level->getWidth(), m_level->getHeight())).size() << "\n";
        }

        m_debugText.setString(debugInfo.str());
    }
}

void GameState::render(sf::RenderWindow& window) {
    /*window.setView(*m_gameView);*/

    if (m_level) {
        m_level->render(window);
    }

    window.setView(window.getDefaultView());

    window.draw(m_healthBarBackground);
    window.draw(m_healthBar);
    window.draw(m_scoreText);

    if (m_showDebugInfo) {
        window.draw(m_debugText);
    }

    if (m_isPaused) {
        sf::Text pauseText;
        pauseText.setFont(m_debugFont);
        pauseText.setCharacterSize(48);
        pauseText.setFillColor(sf::Color::White);
        pauseText.setString("PAUSED");

        sf::FloatRect textBounds = pauseText.getLocalBounds();
        pauseText.setOrigin(textBounds.width / 2, textBounds.height / 2);
        pauseText.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);

        window.draw(pauseText);
    }
}

void GameState::resetLevel() {
    std::cout << "Resetting level..." << std::endl;

    if (m_level) {
        m_level->reset();
    }

    m_gameOver = false;
}

void GameState::updateCamera() {
    if (!m_player || !m_gameView || !m_level) return;

    sf::Vector2f playerPos = m_player->getPosition();

    sf::FloatRect levelBounds = m_level->getCameraBounds();

    sf::Vector2f viewCenter = playerPos;

    float viewWidth = m_gameView->getSize().x;
    float viewHeight = m_gameView->getSize().y;

    if (viewCenter.x - viewWidth / 2 < levelBounds.left) {
        viewCenter.x = levelBounds.left + viewWidth / 2;
    }
    else if (viewCenter.x + viewWidth / 2 > levelBounds.left + levelBounds.width) {
        viewCenter.x = levelBounds.left + levelBounds.width - viewWidth / 2;
    }

    if (viewCenter.y - viewHeight / 2 < levelBounds.top) {
        viewCenter.y = levelBounds.top + viewHeight / 2;
    }
    else if (viewCenter.y + viewHeight / 2 > levelBounds.top + levelBounds.height) {
        viewCenter.y = levelBounds.top + levelBounds.height - viewHeight / 2;
    }

    m_gameView->setCenter(viewCenter);
}

void GameState::updateHUD() {
    if (!m_player) return;

    float healthPercent = static_cast<float>(m_player->getHealth()) / m_player->getMaxHealth();
    m_healthBar.setSize(sf::Vector2f(200.f * healthPercent, 20.f));

    m_scoreText.setString("Score: " + std::to_string(m_score));
}

void GameState::pauseGame() {
    m_isPaused = !m_isPaused;

    if (m_isPaused) {
        std::cout << "Game paused" << std::endl;
    }
    else {
        std::cout << "Game resumed" << std::endl;
    }
}

void GameState::gameOver()
{
    if (m_gameOver) return;

    m_gameOver = true;
    std::cout << "Game over" << std::endl;

    sf::Clock delay;
    while (delay.getElapsedTime().asSeconds() < 2.0f) 
    {
        sf::sleep(sf::milliseconds(100));
    }

    m_game.getStateManager().pushState(StateType::GameOver);
}

void GameState::resume()
{
    m_isPaused = false;
}

void GameState::pause()
{
    m_isPaused = true;
}