#include "StateManager.h"
#include "State.h"
#include "Game.h"
#include <iostream>

StateManager::StateManager(Game& game) : m_game(game) {
}

void StateManager::pushState(StateType stateType) {
    m_pendingChanges.emplace_back(PendingChange::Push, stateType);
}

void StateManager::popState() {
    m_pendingChanges.emplace_back(PendingChange::Pop);
}

void StateManager::clearStates() {
    m_pendingChanges.emplace_back(PendingChange::Clear);
}

State* StateManager::getActiveState() {
    if (m_states.empty()) return nullptr;
    return m_states.back().get();
}

StateType StateManager::getActiveStateType() const {
    if (m_states.empty()) return StateType::None;

    return m_states.back()->getStateType();
}

void StateManager::handleEvent(const sf::Event& event) {
    if (!m_states.empty()) {
        m_states.back()->handleEvent(event);
    }

    applyPendingChanges();
}

void StateManager::update(float dt) {
    if (!m_states.empty()) {
        m_states.back()->update(dt);
    }
    applyPendingChanges();
}

void StateManager::render(sf::RenderWindow& window) {
    for (auto& state : m_states) {
        if (state->isTransparent() && &state != &m_states.back()) {
            state->render(window);
        }
    }

    if (!m_states.empty()) {
        m_states.back()->render(window);
    }
}

bool StateManager::isEmpty() const {
    return m_states.empty();
}

void StateManager::applyPendingChanges() {
    for (auto& change : m_pendingChanges) {
        switch (change.action) {
        case PendingChange::Push: {
            if (m_factories.find(change.stateType) == m_factories.end()) {
                std::cerr << "State factory for type " << static_cast<int>(change.stateType)
                    << " not registered!" << std::endl;
                continue;
            }

            auto state = m_factories[change.stateType](m_game);

            state->initialize();

            m_states.push_back(std::move(state));
            break;
        }

        case PendingChange::Pop:
            if (!m_states.empty()) {
                m_states.pop_back();

                if (!m_states.empty()) {
                    m_states.back()->activate();
                }
            }
            break;

        case PendingChange::Clear:
            m_states.clear();
            break;
        }
    }

    m_pendingChanges.clear();
}