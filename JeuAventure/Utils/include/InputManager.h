#pragma once

#include <SFML/Window.hpp>
#include <map>
#include <functional>
#include <vector>

enum class InputAction {
    MOVE_LEFT,
    MOVE_RIGHT,
    JUMP,
    ATTACK,
    DASH,
    PAUSE,
    MENU,
    CONFIRM,
    CANCEL,
    INTERACT
};

class InputManager {
private:
    static InputManager* s_instance;

    std::map<InputAction, sf::Keyboard::Key> m_keyBindings;

    std::map<sf::Keyboard::Key, bool> m_currentKeyStates;
    std::map<sf::Keyboard::Key, bool> m_previousKeyStates;

    bool m_currentMouseButtons[sf::Mouse::ButtonCount];
    bool m_previousMouseButtons[sf::Mouse::ButtonCount];
    sf::Vector2i m_mousePosition;

    InputManager();

public:
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    static InputManager* getInstance();

    static void cleanup();

    void update(const sf::Window& window);

    void bindKey(InputAction action, sf::Keyboard::Key key);
    sf::Keyboard::Key getKeyBinding(InputAction action);

    bool isActionPressed(InputAction action);
    bool isActionJustPressed(InputAction action);
    bool isActionJustReleased(InputAction action);

    bool isKeyPressed(sf::Keyboard::Key key);
    bool isKeyJustPressed(sf::Keyboard::Key key);
    bool isKeyJustReleased(sf::Keyboard::Key key);

    bool isMouseButtonPressed(sf::Mouse::Button button);
    bool isMouseButtonJustPressed(sf::Mouse::Button button);
    bool isMouseButtonJustReleased(sf::Mouse::Button button);
    sf::Vector2i getMousePosition();

    void setDefaultKeyBindings();
};
