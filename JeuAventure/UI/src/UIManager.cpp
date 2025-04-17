#include "UIManager.h"
#include <iostream>

UIManager* UIManager::s_instance = nullptr;

UIManager::UIManager()
    : m_screenSize(800.0f, 600.0f),
    m_focusedElement(nullptr),
    m_hoveredElement(nullptr),
    m_active(true),
    m_showHUD(true),
    m_inDialog(false),
    m_inMenu(false),
    m_modalActive(false),
    m_transitionTimer(0.0f),
    m_transitionDuration(0.0f),
    m_transitionType(""),
    m_backgroundColor(sf::Color(30, 30, 30)),
    m_textColor(sf::Color::White),
    m_accentColor(sf::Color(100, 150, 255)),
    m_defaultFont("arial")
{
    m_hud = std::make_unique<HUD>();
    m_dialogSystem = std::make_unique<DialogSystem>();
}

UIManager* UIManager::getInstance() {
    if (s_instance == nullptr) {
        s_instance = new UIManager();
    }
    return s_instance;
}

void UIManager::cleanup() {
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}

void UIManager::initialize(const sf::Vector2f& screenSize) {
    m_screenSize = screenSize;
    m_hud->initialize();
    m_hud->setSize(m_screenSize);
    std::cout << "UIManager initialized with screen size: "
        << m_screenSize.x << "x" << m_screenSize.y << std::endl;
}

void UIManager::setScreenSize(const sf::Vector2f& size) {
    m_screenSize = size;
    if (m_hud) {
        m_hud->setSize(m_screenSize);
    }
    for (auto& [id, element] : m_uiElements) {
        if (element) {
            element->recalculatePosition();
        }
    }
    if (!m_menuStack.empty() && m_menuStack.top()) {
        m_menuStack.top()->setSize(m_screenSize);
    }
}

sf::Vector2f UIManager::getScreenSize() const {
    return m_screenSize;
}

void UIManager::update(float dt) {
    if (m_transitionTimer > 0.0f) {
        m_transitionTimer -= dt;
        if (m_transitionTimer <= 0.0f) {
            m_transitionTimer = 0.0f;
            if (m_transitionCallback) {
                m_transitionCallback();
                m_transitionCallback = nullptr;
            }
        }
    }
    if (m_active) {
        if (m_inDialog && m_dialogSystem) {
            m_dialogSystem->update(dt);
        }
        else if (m_inMenu && !m_menuStack.empty() && m_menuStack.top()) {
            m_menuStack.top()->update(dt);
        }
        else if (m_showHUD && m_hud) {
            m_hud->update(dt);
        }
        for (auto& [id, element] : m_uiElements) {
            if (element && element->isActive()) {
                element->update(dt);
            }
        }
    }
}

void UIManager::render(sf::RenderTarget& target) {
    if (!m_active) return;
    if (m_showHUD && m_hud) {
        m_hud->render(target);
    }
    for (auto& [id, element] : m_uiElements) {
        if (element && element->isVisible()) {
            element->render(target);
        }
    }
    if (m_inMenu && !m_menuStack.empty() && m_menuStack.top()) {
        m_menuStack.top()->render(target);
    }
    if (m_inDialog && m_dialogSystem) {
        m_dialogSystem->render(target);
    }
}

bool UIManager::handleEvent(const sf::Event& event) {
    if (!m_active) return false;
    if (m_inDialog && m_dialogSystem) {
        if (m_dialogSystem->handleEvent(event)) {
            return true;
        }
    }
    if (m_inMenu && !m_menuStack.empty() && m_menuStack.top()) {
        if (m_menuStack.top()->handleEvent(event)) {
            return true;
        }
    }
    for (auto it = m_uiElements.rbegin(); it != m_uiElements.rend(); ++it) {
        if (it->second && it->second->isActive() && it->second->isInteractive()) {
            if (it->second->handleEvent(event)) {
                return true;
            }
        }
    }
    if (m_showHUD && m_hud) {
        if (m_hud->handleEvent(event)) {
            return true;
        }
    }
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x),
            static_cast<float>(event.mouseMove.y));
        UIElement* newHovered = findElementAt(mousePos);
        if (newHovered != m_hoveredElement) {
            if (m_hoveredElement) {
                m_hoveredElement->hover(false);
            }
            m_hoveredElement = newHovered;
            if (m_hoveredElement) {
                m_hoveredElement->hover(true);
            }
        }
    }
    return false;
}

HUD* UIManager::getHUD() {
    return m_hud.get();
}

void UIManager::showHUD(bool show) {
    m_showHUD = show;
}

bool UIManager::isHUDVisible() const {
    return m_showHUD;
}

void UIManager::addUIElement(std::unique_ptr<UIElement> element) {
    if (!element) return;
    std::string id = element->getID();
    if (id.empty()) {
        static int nextId = 1;
        id = "UIElement_" + std::to_string(nextId++);
        element->setID(id);
    }
    m_uiElements[id] = std::move(element);
}

UIElement* UIManager::getUIElement(const std::string& id) {
    auto it = m_uiElements.find(id);
    if (it != m_uiElements.end()) {
        return it->second.get();
    }
    return nullptr;
}

void UIManager::removeUIElement(const std::string& id) {
    auto it = m_uiElements.find(id);
    if (it != m_uiElements.end()) {
        if (m_focusedElement == it->second.get()) {
            clearFocus();
        }
        if (m_hoveredElement == it->second.get()) {
            clearHovered();
        }
        m_uiElements.erase(it);
    }
}

void UIManager::clearUIElements() {
    clearFocus();
    clearHovered();
    m_uiElements.clear();
}

void UIManager::pushMenu(Menu* menu) {
    if (!menu) return;
    m_menuStack.push(menu);
    m_inMenu = true;
    menu->setSize(m_screenSize);
    menu->startAnimation();
}

void UIManager::popMenu() {
    if (m_menuStack.empty()) return;
    m_menuStack.pop();
    m_inMenu = !m_menuStack.empty();
    if (m_inMenu) {
        m_menuStack.top()->startAnimation();
    }
}

void UIManager::clearMenus() {
    while (!m_menuStack.empty()) {
        m_menuStack.pop();
    }
    m_inMenu = false;
}

Menu* UIManager::getCurrentMenu() {
    if (m_menuStack.empty()) return nullptr;
    return m_menuStack.top();
}

bool UIManager::isInMenu() const {
    return m_inMenu;
}

DialogSystem* UIManager::getDialogSystem() {
    return m_dialogSystem.get();
}

void UIManager::startDialog(const std::string& dialogID) {
    if (m_dialogSystem) {
        m_dialogSystem->startDialog(dialogID);
        m_inDialog = true;
    }
}

void UIManager::endDialog() {
    if (m_dialogSystem) {
        m_dialogSystem->endDialog();
        m_inDialog = false;
    }
}

bool UIManager::isInDialog() const {
    return m_inDialog;
}

void UIManager::startTransition(const std::string& type, float duration, const std::function<void()>& callback) {
    m_transitionType = type;
    m_transitionDuration = duration;
    m_transitionTimer = duration;
    m_transitionCallback = callback;
}

bool UIManager::isInTransition() const {
    return m_transitionTimer > 0.0f;
}

void UIManager::setFocus(UIElement* element) {
    if (m_focusedElement == element) return;
    clearFocus();
    m_focusedElement = element;
    if (m_focusedElement) {
        m_focusedElement->focus();
    }
}

UIElement* UIManager::getFocusedElement() const {
    return m_focusedElement;
}

void UIManager::clearFocus() {
    if (m_focusedElement) {
        m_focusedElement->unfocus();
        m_focusedElement = nullptr;
    }
}

void UIManager::setHovered(UIElement* element) {
    if (m_hoveredElement == element) return;
    if (m_hoveredElement) {
        m_hoveredElement->hover(false);
    }
    m_hoveredElement = element;
    if (m_hoveredElement) {
        m_hoveredElement->hover(true);
    }
}

UIElement* UIManager::getHoveredElement() const {
    return m_hoveredElement;
}

void UIManager::clearHovered() {
    if (m_hoveredElement) {
        m_hoveredElement->hover(false);
        m_hoveredElement = nullptr;
    }
}

void UIManager::setActive(bool active) {
    m_active = active;
}

bool UIManager::isActive() const {
    return m_active;
}

void UIManager::setModalActive(bool active) {
    m_modalActive = active;
}

bool UIManager::isModalActive() const {
    return m_modalActive;
}

void UIManager::setBackgroundColor(const sf::Color& color) {
    m_backgroundColor = color;
}

void UIManager::setTextColor(const sf::Color& color) {
    m_textColor = color;
}

void UIManager::setAccentColor(const sf::Color& color) {
    m_accentColor = color;
}

void UIManager::setDefaultFont(const std::string& fontPath) {
    m_defaultFont = fontPath;
}

void UIManager::showNotification(const std::string& text, float duration) {
    auto panel = std::make_unique<Panel>("notification", sf::Vector2f(m_screenSize.x / 2, m_screenSize.y - 100), sf::Vector2f(300, 50));
    panel->setDrawBackground(true);
    panel->setBackgroundColor(sf::Color(50, 50, 50, 200));
    panel->setBorder(true);
    panel->setBorderColor(m_accentColor);
    panel->setAnchor(UIAnchor::Bottom);
    auto label = std::make_unique<Label>("notification_text", text, sf::Vector2f(150, 25));
    label->setTextColor(m_textColor);
    label->setAnchor(UIAnchor::Center);
    panel->addChild(std::move(label));
    addUIElement(std::move(panel));
    startTransition("notification", duration, [this]() {
        removeUIElement("notification");
        });
}

void UIManager::clearNotifications() {
    auto it = m_uiElements.begin();
    while (it != m_uiElements.end()) {
        if (it->first.find("notification") == 0) {
            it = m_uiElements.erase(it);
        }
        else {
            ++it;
        }
    }
}

UIElement* UIManager::findElementAt(const sf::Vector2f& position) {
    if (m_inDialog && m_dialogSystem && m_dialogSystem->isVisible() &&
        m_dialogSystem->isActive() && m_dialogSystem->containsPoint(position)) {
        return m_dialogSystem.get();
    }
    if (m_inMenu && !m_menuStack.empty() && m_menuStack.top() &&
        m_menuStack.top()->isVisible() && m_menuStack.top()->isActive() &&
        m_menuStack.top()->containsPoint(position)) {
        return m_menuStack.top();
    }
    for (auto it = m_uiElements.rbegin(); it != m_uiElements.rend(); ++it) {
        if (it->second && it->second->isVisible() && it->second->isActive() &&
            it->second->isInteractive() && it->second->containsPoint(position)) {
            return it->second.get();
        }
    }
    if (m_showHUD && m_hud && m_hud->isVisible() && m_hud->isActive() &&
        m_hud->isInteractive() && m_hud->containsPoint(position)) {
        return m_hud.get();
    }
    return nullptr;
}
