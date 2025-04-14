#pragma once

#include "UIElements.h"
#include "HUD.h"
#include "Menu.h"
#include "DialogSystem.h"
#include <map>
#include <stack>
#include <memory>
#include <string>

class UIManager {
private:
    static UIManager* s_instance;

    sf::Vector2f m_screenSize;

    std::unique_ptr<HUD> m_hud;

    std::unique_ptr<DialogSystem> m_dialogSystem;

    std::map<std::string, std::unique_ptr<UIElements>> m_uiElements;

    std::stack<Menu*> m_menuStack;

    UIElements* m_focusedElement;

    UIElements* m_hoveredElement;

    bool m_active;
    bool m_showHUD;
    bool m_inDialog;
    bool m_inMenu;
    bool m_modalActive;

    float m_transitionTimer;
    float m_transitionDuration;
    std::string m_transitionType;
    std::function<void()> m_transitionCallback;

    sf::Color m_backgroundColor;
    sf::Color m_textColor;
    sf::Color m_accentColor;

    std::string m_defaultFont;

    UIManager();

    UIElements* findElementAt(const sf::Vector2f& position);

public:
    static UIManager* getInstance();
    static void cleanup();

    void initialize(const sf::Vector2f& screenSize);
    void setScreenSize(const sf::Vector2f& size);
    sf::Vector2f getScreenSize() const;

    void update(float dt);
    void render(sf::RenderTarget& target);
    bool handleEvent(const sf::Event& event);

    HUD* getHUD();
    void showHUD(bool show);
    bool isHUDVisible() const;

    void addUIElement(std::unique_ptr<UIElements> element);
    UIElements* getUIElement(const std::string& id);
    void removeUIElement(const std::string& id);
    void clearUIElements();

    void pushMenu(Menu* menu);
    void popMenu();
    void clearMenus();
    Menu* getCurrentMenu();
    bool isInMenu() const;

    DialogSystem* getDialogSystem();
    void startDialog(const std::string& dialogID);
    void endDialog();
    bool isInDialog() const;

    void startTransition(const std::string& type, float duration, const std::function<void()>& callback);
    bool isInTransition() const;

    void setFocus(UIElements* element);
    UIElements* getFocusedElement() const;
    void clearFocus();

    void setHovered(UIElements* element);
    UIElements* getHoveredElement() const;
    void clearHovered();

    void setActive(bool active);
    bool isActive() const;
    void setModalActive(bool active);
    bool isModalActive() const;

    void setBackgroundColor(const sf::Color& color);
    void setTextColor(const sf::Color& color);
    void setAccentColor(const sf::Color& color);
    void setDefaultFont(const std::string& fontPath);

    void showNotification(const std::string& text, float duration = 3.0f);
    void clearNotifications();
};
