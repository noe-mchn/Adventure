#include "GameState.h"
#include "Game.h"
#include "RessourceManager.h"
#include "InputManager.h"
#include "EventSystem.h"
#include <iostream>
#include <sstream>

class Player {
public:
    Player() {}
    void update(float dt) {}
    void render(sf::RenderWindow& window) {}
    void setPosition(float x, float y) {}
    sf::Vector2f getPosition() const { return sf::Vector2f(0, 0); }
    sf::FloatRect getBounds() const { return sf::FloatRect(); }
    bool isAlive() const { return true; }
    int getHealth() const { return 100; }
    int getMaxHealth() const { return 100; }
};

class Enemy {
public:
    Enemy(float x, float y, const std::string& type) {}
    void update(float dt, const Player& player) {}
    void render(sf::RenderWindow& window) {}
    sf::FloatRect getBounds() const { return sf::FloatRect(); }
    bool isAlive() const { return true; }
};

class Level {
public:
    Level() {}
    void loadFromFile(const std::string& filename) {}
    void update(float dt) {}
    void render(sf::RenderWindow& window) {}
    sf::FloatRect getBounds() const { return sf::FloatRect(0, 0, 2000, 1000); }
    sf::Vector2f getPlayerStart() const { return sf::Vector2f(100, 100); }
    std::vector<sf::Vector2f> getEnemyPositions(const std::string& type) const { return {}; }
    bool checkCollision(const sf::FloatRect& bounds) const { return false; }
};

GameState::GameState(Game& game)
    : State(game, StateType::Game),
    m_showDebugInfo(false),
    m_score(0),
    m_currentLevel(1),
    m_gameOver(false)
{
}

void GameState::initialize() {
    auto* resourceManager = RessourceManager::getInstance();

    if (!resourceManager->loadFont("debug_font", "Assets/Fonts/debug.ttf")) {
        std::cerr << "Failed to load debug font!" << std::endl;
        m_debugFont = sf::Font();
        if (!m_debugFont.loadFromFile("Assets/Fonts/debug.ttf")) {
            std::cerr << "Failed to load fallback font!" << std::endl;
        }
    }
    else {
        m_debugFont = *resourceManager->getFont("debug_font");
    }

    m_debugText.setFont(m_debugFont);
    m_debugText.setCharacterSize(14);
    m_debugText.setFillColor(sf::Color::Yellow);
    m_debugText.setPosition(10.f, 10.f);

    m_scoreText.setFont(m_debugFont);
    m_scoreText.setCharacterSize(24);
    m_scoreText.setFillColor(sf::Color::White);

    m_healthBarBackground.setSize(sf::Vector2f(200.f, 20.f));
    m_healthBarBackground.setFillColor(sf::Color(100, 100, 100, 200));
    m_healthBarBackground.setOutlineColor(sf::Color::Black);
    m_healthBarBackground.setOutlineThickness(2.f);

    m_healthBar.setSize(sf::Vector2f(200.f, 20.f));
    m_healthBar.setFillColor(sf::Color(255, 50, 50, 200));

    m_gameView = std::make_unique<sf::View>(sf::FloatRect(0.f, 0.f,
        static_cast<float>(m_game.getWindow().getSize().x),
        static_cast<float>(m_game.getWindow().getSize().y)));

    m_player = std::make_unique<Player>();

    m_level = std::make_unique<Level>();
    loadLevel(m_currentLevel);

    EventSystem::getInstance()->addEventListener("PlayerDeath", [this](const std::map<std::string, std::any>&) {
        gameOver();
        });

    resetGame();
}

void GameState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::F3) {
            m_showDebugInfo = !m_showDebugInfo;
        }
        else if (event.key.code == sf::Keyboard::Escape) {
            pauseGame();
        }
    }
}

void GameState::update(float dt) {
    if (m_gameOver) return;

    if (m_player) {
        m_player->update(dt);
    }

    if (m_level) {
        m_level->update(dt);
    }

    for (auto& enemy : m_enemies) {
        if (m_player && enemy) {
            enemy->update(dt, *m_player);
        }
    }

    checkCollisions();
    updateCamera();
    updateHUD();

    if (m_showDebugInfo) {
        std::ostringstream debugInfo;
        debugInfo << "FPS: " << static_cast<int>(1.0f / dt) << "\n";
        debugInfo << "Entities: " << (1 + m_enemies.size()) << "\n";

        if (m_player) {
            sf::Vector2f playerPos = m_player->getPosition();
            debugInfo << "Player Pos: " << playerPos.x << ", " << playerPos.y << "\n";
        }

        m_debugText.setString(debugInfo.str());
    }

    if (m_enemies.empty()) {
        m_currentLevel++;
        loadLevel(m_currentLevel);
    }

    if (m_player && !m_player->isAlive()) {
        gameOver();
    }
}

void GameState::render(sf::RenderWindow& window) {
    window.setView(*m_gameView);

    if (m_level) {
        m_level->render(window);
    }

    for (const auto& enemy : m_enemies) {
        if (enemy) {
            enemy->render(window);
        }
    }

    if (m_player) {
        m_player->render(window);
    }

    window.setView(window.getDefaultView());

    window.draw(m_healthBarBackground);
    window.draw(m_healthBar);
    window.draw(m_scoreText);

    if (m_showDebugInfo) {
        window.draw(m_debugText);
    }
}

void GameState::resetGame() {
    m_score = 0;
    m_currentLevel = 1;
    m_gameOver = false;

    if (m_player && m_level) {
        sf::Vector2f startPos = m_level->getPlayerStart();
        m_player->setPosition(startPos.x, startPos.y);
    }

    m_enemies.clear();
    loadLevel(m_currentLevel);
    updateHUD();
}

void GameState::loadLevel(int levelIndex) {
    if (!m_level) return;

    std::string levelFile = "Assets/Levels/level" + std::to_string(levelIndex) + ".txt";
    m_level->loadFromFile(levelFile);

    sf::Vector2f startPos = m_level->getPlayerStart();
    if (m_player) {
        m_player->setPosition(startPos.x, startPos.y);
    }

    m_enemies.clear();

    std::vector<sf::Vector2f> basicEnemyPositions = m_level->getEnemyPositions("basic");
    for (const auto& pos : basicEnemyPositions) {
        spawnEnemy(pos.x, pos.y, "basic");
    }

    std::vector<sf::Vector2f> flyingEnemyPositions = m_level->getEnemyPositions("flying");
    for (const auto& pos : flyingEnemyPositions) {
        spawnEnemy(pos.x, pos.y, "flying");
    }
}

void GameState::updateCamera() {
    if (!m_player || !m_gameView) return;

    sf::Vector2f playerPos = m_player->getPosition();
    sf::FloatRect levelBounds = m_level ? m_level->getBounds() : sf::FloatRect(0, 0, 2000, 1000);
    sf::Vector2f viewCenter = playerPos;

    float viewWidth = m_gameView->getSize().x;
    float viewHeight = m_gameView->getSize().y;

    if (viewCenter.x - viewWidth / 2 < levelBounds.left) {
        viewCenter.x = levelBounds.left + viewWidth / 2;
    }
    if (viewCenter.x + viewWidth / 2 > levelBounds.left + levelBounds.width) {
        viewCenter.x = levelBounds.left + levelBounds.width - viewWidth / 2;
    }
    if (viewCenter.y - viewHeight / 2 < levelBounds.top) {
        viewCenter.y = levelBounds.top + viewHeight / 2;
    }
    if (viewCenter.y + viewHeight / 2 > levelBounds.top + levelBounds.height) {
        viewCenter.y = levelBounds.top + levelBounds.height - viewHeight / 2;
    }

    m_gameView->setCenter(viewCenter);
}

void GameState::updateHUD() {
    if (!m_player) return;

    float healthPercent = static_cast<float>(m_player->getHealth()) / m_player->getMaxHealth();
    m_healthBar.setSize(sf::Vector2f(200.f * healthPercent, 20.f));

    m_healthBarBackground.setPosition(20.f, 20.f);
    m_healthBar.setPosition(20.f, 20.f);

    m_scoreText.setString("Score: " + std::to_string(m_score));
    m_scoreText.setPosition(20.f, 50.f);
}

void GameState::pauseGame() {
    EventSystem::getInstance()->triggerEvent("PauseGame");
}

void GameState::gameOver() {
    if (m_gameOver) return;

    m_gameOver = true;
    m_game.getStateManager().pushState(StateType::GameOver);
}

void GameState::respawnPlayer() {
    if (!m_player || !m_level) return;

    sf::Vector2f startPos = m_level->getPlayerStart();
    m_player->setPosition(startPos.x, startPos.y);
}

void GameState::spawnEnemy(float x, float y, const std::string& type) {
    m_enemies.push_back(std::make_unique<Enemy>(x, y, type));
}

void GameState::checkCollisions() {
    if (!m_player || !m_level) return;
}
