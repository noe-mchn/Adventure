#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include "State.h"

class Game;


typedef std::unique_ptr<State>(*StateFactory)(Game& game);

class StateManager {
private:
    
    Game& m_game;

    
    std::map<StateType, StateFactory> m_factories;

    std::vector<std::unique_ptr<State>> m_states;

    struct PendingChange {
        enum Action {
            Push, Pop, Clear
        };

        Action action;
        StateType stateType;

        PendingChange(Action action, StateType stateType = StateType::None) :
            action(action), stateType(stateType) {
        }
    };

    std::vector<PendingChange> m_pendingChanges;

    void applyPendingChanges();

public:
    StateManager(Game& game);

    template <typename T>
    void registerState(StateType stateType);

    void pushState(StateType stateType);
    void popState();
    void clearStates();

    State* getActiveState();
    StateType getActiveStateType() const;

    void handleEvent(const sf::Event& event);
    void update(float dt);
    void render(sf::RenderWindow& window);

    bool isEmpty() const;
};

template <typename T>
void StateManager::registerState(StateType stateType) {
    m_factories[stateType] = [](Game& game) -> std::unique_ptr<State> {
        return std::make_unique<T>(game);
        };
}
