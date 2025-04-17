#pragma once

#include "State.h"
#include <SFML/Graphics.hpp>
#include <memory>

class Player;
class Level;
class Game;

class GameState : public State {
public:
    GameState(Game& game);
    ~GameState();

    void initialize() override;
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    void pause();
    void resume();

private:
    void resetLevel();
    void updateCamera();
    void updateHUD();
    void pauseGame();
    void gameOver();

    std::unique_ptr<sf::View> m_gameView;
    std::unique_ptr<Player> m_player;
    std::unique_ptr<Level> m_level;

    sf::Font m_debugFont;
    sf::Text m_debugText;
    sf::Text m_scoreText;

    sf::RectangleShape m_healthBarBackground;
    sf::RectangleShape m_healthBar;

    bool m_showDebugInfo;
    int m_score;
    bool m_gameOver;
    bool m_isPaused;
};