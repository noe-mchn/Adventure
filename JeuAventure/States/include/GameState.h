#pragma once

#include "State.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class Player;
class Enemy;
class Level;
namespace sf {
    class View;
}

class GameState : public State {
private:
    std::unique_ptr<sf::View> m_gameView;
    std::unique_ptr<Player> m_player;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::unique_ptr<Level> m_level;

    bool m_showDebugInfo;
    sf::Text m_debugText;
    sf::Font m_debugFont;

    sf::RectangleShape m_healthBar;
    sf::RectangleShape m_healthBarBackground;
    sf::Text m_scoreText;

    int m_score;
    int m_currentLevel;
    bool m_gameOver;

    void updateCamera();
    void loadLevel(int levelIndex);
    void updateHUD();
    void respawnPlayer();
    void spawnEnemy(float x, float y, const std::string& type);
    void checkCollisions();

public:
    GameState(Game& game);
    virtual ~GameState() = default;

    void initialize() override;
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    void resetGame();
    void pauseGame();
    void gameOver();
};