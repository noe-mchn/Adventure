#include "State.h"
#include "Game.h"
#include "RessourceManager.h"
#include "EventSystem.h"

State::State(Game& game, StateType type)
    : m_game(game), m_transparent(false), m_active(true), m_stateType(type) {
}

void State::activate() {
    m_active = true;
}

void State::deactivate() {
    m_active = false;
}

bool State::isTransparent() const {
    return m_transparent;
}

bool State::isActive() const {
    return m_active;
}

StateType State::getStateType() const {
    return m_stateType;
}