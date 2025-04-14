#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

class Game;
class StateManager;

enum class StateType {
    None,
    Menu,
    Game,
    Pause,
    GameOver
};

class State {
protected:
    Game& m_game;
    bool m_transparent;
    bool m_active;
    StateType m_stateType;

public:
    State(Game& game, StateType type);
    virtual ~State() = default;

    virtual void initialize() = 0;
    virtual void activate();
    virtual void deactivate();

    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;

    bool isTransparent() const;
    bool isActive() const;
    StateType getStateType() const;
};