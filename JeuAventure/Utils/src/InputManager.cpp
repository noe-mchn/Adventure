#include "InputManager.h"

InputManager* InputManager::s_instance = nullptr;

InputManager::InputManager() {
    for (int i = 0; i < sf::Keyboard::KeyCount; ++i) {
        sf::Keyboard::Key key = static_cast<sf::Keyboard::Key>(i);
        m_currentKeyStates[key] = false;
        m_previousKeyStates[key] = false;
    }

    for (int i = 0; i < sf::Mouse::ButtonCount; ++i) {
        m_currentMouseButtons[i] = false;
        m_previousMouseButtons[i] = false;
    }

    setDefaultKeyBindings();
}

InputManager* InputManager::getInstance() {
    if (s_instance == nullptr) {
        s_instance = new InputManager();
    }
    return s_instance;
}

void InputManager::cleanup() {
    if (s_instance != nullptr) {
        delete s_instance;
        s_instance = nullptr;
    }
}

void InputManager::update(const sf::Window& window) {
    m_previousKeyStates = m_currentKeyStates;

    for (auto& [key, state] : m_currentKeyStates) {
        m_currentKeyStates[key] = sf::Keyboard::isKeyPressed(key);
    }

    for (int i = 0; i < sf::Mouse::ButtonCount; ++i) {
        m_previousMouseButtons[i] = m_currentMouseButtons[i];
    }

    for (int i = 0; i < sf::Mouse::ButtonCount; ++i) {
        m_currentMouseButtons[i] = sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(i));
    }

    m_mousePosition = sf::Mouse::getPosition(window);
}

void InputManager::bindKey(InputAction action, sf::Keyboard::Key key) {
    m_keyBindings[action] = key;
}

sf::Keyboard::Key InputManager::getKeyBinding(InputAction action) {
    auto it = m_keyBindings.find(action);
    if (it != m_keyBindings.end()) {
        return it->second;
    }
    return sf::Keyboard::Unknown;
}

bool InputManager::isActionPressed(InputAction action) {
    auto it = m_keyBindings.find(action);
    if (it != m_keyBindings.end()) {
        return m_currentKeyStates[it->second];
    }
    return false;
}

bool InputManager::isActionJustPressed(InputAction action) {
    auto it = m_keyBindings.find(action);
    if (it != m_keyBindings.end()) {
        return m_currentKeyStates[it->second] && !m_previousKeyStates[it->second];
    }
    return false;
}

bool InputManager::isActionJustReleased(InputAction action) {
    auto it = m_keyBindings.find(action);
    if (it != m_keyBindings.end()) {
        return !m_currentKeyStates[it->second] && m_previousKeyStates[it->second];
    }
    return false;
}

bool InputManager::isKeyPressed(sf::Keyboard::Key key) {
    return m_currentKeyStates[key];
}

bool InputManager::isKeyJustPressed(sf::Keyboard::Key key) {
    return m_currentKeyStates[key] && !m_previousKeyStates[key];
}

bool InputManager::isKeyJustReleased(sf::Keyboard::Key key) {
    return !m_currentKeyStates[key] && m_previousKeyStates[key];
}

bool InputManager::isMouseButtonPressed(sf::Mouse::Button button) {
    return m_currentMouseButtons[button];
}

bool InputManager::isMouseButtonJustPressed(sf::Mouse::Button button) {
    return m_currentMouseButtons[button] && !m_previousMouseButtons[button];
}

bool InputManager::isMouseButtonJustReleased(sf::Mouse::Button button) {
    return !m_currentMouseButtons[button] && m_previousMouseButtons[button];
}

sf::Vector2i InputManager::getMousePosition() {
    return m_mousePosition;
}

void InputManager::setDefaultKeyBindings() {
    bindKey(InputAction::MOVE_LEFT, sf::Keyboard::Left);
    bindKey(InputAction::MOVE_RIGHT, sf::Keyboard::Right);
    bindKey(InputAction::JUMP, sf::Keyboard::Space);

    bindKey(InputAction::ATTACK, sf::Keyboard::X);
    bindKey(InputAction::DASH, sf::Keyboard::C);

    bindKey(InputAction::PAUSE, sf::Keyboard::Escape);
    bindKey(InputAction::MENU, sf::Keyboard::Tab);
    bindKey(InputAction::CONFIRM, sf::Keyboard::Return);
    bindKey(InputAction::CANCEL, sf::Keyboard::Escape);
}