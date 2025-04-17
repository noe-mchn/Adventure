#include "UIElements.h"
#include <algorithm>
#include <iostream>

UIElement::UIElement()
    : m_id(""),
    m_visible(true),
    m_active(true),
    m_interactive(true),
    m_position(0.0f, 0.0f),
    m_size(100.0f, 100.0f),
    m_scale(1.0f, 1.0f),
    m_rotation(0.0f),
    m_parent(nullptr),
    m_anchor(UIAnchor::TopLeft),
    m_scaleMode(UIScaleMode::None),
    m_anchorOffset(0.0f, 0.0f),
    m_color(sf::Color::White),
    m_opacity(1.0f),
    m_animationSpeed(1.0f)
{
}

UIElement::UIElement(const std::string& id, const sf::Vector2f& position, const sf::Vector2f& size)
    : m_id(id),
    m_visible(true),
    m_active(true),
    m_interactive(true),
    m_position(position),
    m_size(size),
    m_scale(1.0f, 1.0f),
    m_rotation(0.0f),
    m_parent(nullptr),
    m_anchor(UIAnchor::TopLeft),
    m_scaleMode(UIScaleMode::None),
    m_anchorOffset(0.0f, 0.0f),
    m_color(sf::Color::White),
    m_opacity(1.0f),
    m_animationSpeed(1.0f)
{
}

UIElement::~UIElement() {
}

void UIElement::update(float dt) {
    for (auto& child : m_children) {
        if (child && child->isActive()) {
            child->update(dt);
        }
    }
}

void UIElement::render(sf::RenderTarget& target) {
    if (!m_visible) return;

    for (auto& child : m_children) {
        if (child && child->isVisible()) {
            child->render(target);
        }
    }
}

bool UIElement::handleEvent(const sf::Event& event) {
    if (!m_active || !m_interactive) return false;

    if (event.type == sf::Event::MouseButtonPressed ||
        event.type == sf::Event::MouseButtonReleased) {

        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x),
            static_cast<float>(event.mouseButton.y));

        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left &&
            containsPoint(mousePos)) {

            click();
            return true;
        }
    }

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x),
            static_cast<float>(event.mouseMove.y));

        bool isHovering = containsPoint(mousePos);
        hover(isHovering);
    }

    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        auto& child = *it;
        if (child && child->isActive() && child->isInteractive()) {
            if (child->handleEvent(event)) {
                return true;
            }
        }
    }

    return false;
}

void UIElement::addChild(std::unique_ptr<UIElement> child) {
    if (!child) return;

    child->setParent(this);
    m_children.push_back(std::move(child));
    updateChildrenPositions();
}

UIElement* UIElement::getChild(const std::string& id) {
    for (auto& child : m_children) {
        if (child && child->getID() == id) {
            return child.get();
        }

        UIElement* found = child->getChild(id);
        if (found) return found;
    }

    return nullptr;
}

void UIElement::removeChild(const std::string& id) {
    auto it = std::find_if(m_children.begin(), m_children.end(),
        [&id](const auto& child) { return child && child->getID() == id; });

    if (it != m_children.end()) {
        m_children.erase(it);
    }
}

void UIElement::clearChildren() {
    m_children.clear();
}

void UIElement::setPosition(const sf::Vector2f& position) {
    m_position = position;
    recalculatePosition();
}

sf::Vector2f UIElement::getPosition() const {
    return m_position;
}

sf::Vector2f UIElement::getGlobalPosition() const {
    sf::Vector2f globalPos = m_position;

    if (m_parent) {
        globalPos += m_parent->getGlobalPosition();
    }

    return globalPos;
}

void UIElement::setSize(const sf::Vector2f& size) {
    m_size = size;
    updateChildrenPositions();
}

sf::Vector2f UIElement::getSize() const {
    return m_size;
}

void UIElement::setScale(const sf::Vector2f& scale) {
    m_scale = scale;
    updateChildrenPositions();
}

sf::Vector2f UIElement::getScale() const {
    return m_scale;
}

void UIElement::setRotation(float rotation) {
    m_rotation = rotation;
}

float UIElement::getRotation() const {
    return m_rotation;
}

void UIElement::setAnchor(UIAnchor anchor) {
    m_anchor = anchor;
    recalculatePosition();
}

UIAnchor UIElement::getAnchor() const {
    return m_anchor;
}

void UIElement::setScaleMode(UIScaleMode mode) {
    m_scaleMode = mode;
    updateChildrenPositions();
}

UIScaleMode UIElement::getScaleMode() const {
    return m_scaleMode;
}

void UIElement::setAnchorOffset(const sf::Vector2f& offset) {
    m_anchorOffset = offset;
    recalculatePosition();
}

sf::Vector2f UIElement::getAnchorOffset() const {
    return m_anchorOffset;
}

void UIElement::setColor(const sf::Color& color) {
    m_color = color;
    m_color.a = static_cast<sf::Uint8>(255 * m_opacity);
}

sf::Color UIElement::getColor() const {
    return m_color;
}

void UIElement::setOpacity(float opacity) {
    m_opacity = std::max(0.0f, std::min(1.0f, opacity));
    sf::Color color = m_color;
    color.a = static_cast<sf::Uint8>(255 * m_opacity);
    setColor(color);
}

float UIElement::getOpacity() const {
    return m_opacity;
}

void UIElement::setVisible(bool visible) {
    m_visible = visible;
}

bool UIElement::isVisible() const {
    return m_visible;
}

void UIElement::setActive(bool active) {
    m_active = active;
}

bool UIElement::isActive() const {
    return m_active;
}

void UIElement::setInteractive(bool interactive) {
    m_interactive = interactive;
}

bool UIElement::isInteractive() const {
    return m_interactive;
}

void UIElement::setID(const std::string& id) {
    m_id = id;
}

const std::string& UIElement::getID() const {
    return m_id;
}

void UIElement::setParent(UIElement* parent) {
    m_parent = parent;
    recalculatePosition();
}

UIElement* UIElement::getParent() const {
    return m_parent;
}

void UIElement::setOnClickCallback(const std::function<void(UIElement*)>& callback) {
    m_onClickCallback = callback;
}

void UIElement::setOnHoverCallback(const std::function<void(UIElement*)>& callback) {
    m_onHoverCallback = callback;
}

void UIElement::setOnFocusCallback(const std::function<void(UIElement*, bool)>& callback) {
    m_onFocusCallback = callback;
}

void UIElement::focus() {
    if (m_onFocusCallback) {
        m_onFocusCallback(this, true);
    }
}

void UIElement::unfocus() {
    if (m_onFocusCallback) {
        m_onFocusCallback(this, false);
    }
}

void UIElement::click() {
    if (m_onClickCallback) {
        m_onClickCallback(this);
    }
}

void UIElement::hover(bool isHovering) {
    if (m_onHoverCallback && isHovering) {
        m_onHoverCallback(this);
    }
}

bool UIElement::containsPoint(const sf::Vector2f& point) const {
    sf::Vector2f globalPos = getGlobalPosition();

    float left = globalPos.x;
    float top = globalPos.y;
    float right = left + m_size.x * m_scale.x;
    float bottom = top + m_size.y * m_scale.y;

    return (point.x >= left && point.x <= right &&
        point.y >= top && point.y <= bottom);
}

void UIElement::recalculatePosition() {
    sf::Vector2f parentPos(0.0f, 0.0f);
    sf::Vector2f parentSize(0.0f, 0.0f);

    if (m_parent) {
        parentPos = m_parent->getPosition();
        parentSize = m_parent->getSize();
    }

    sf::Vector2f anchoredPosition = m_position;

    switch (m_anchor) {
    case UIAnchor::TopLeft:
        break;

    case UIAnchor::Top:
        anchoredPosition.x = parentSize.x / 2.0f - (m_size.x * m_scale.x) / 2.0f + m_position.x;
        break;

    case UIAnchor::TopRight:
        anchoredPosition.x = parentSize.x - (m_size.x * m_scale.x) + m_position.x;
        break;

    case UIAnchor::Left:
        anchoredPosition.y = parentSize.y / 2.0f - (m_size.y * m_scale.y) / 2.0f + m_position.y;
        break;

    case UIAnchor::Center:
        anchoredPosition.x = parentSize.x / 2.0f - (m_size.x * m_scale.x) / 2.0f + m_position.x;
        anchoredPosition.y = parentSize.y / 2.0f - (m_size.y * m_scale.y) / 2.0f + m_position.y;
        break;

    case UIAnchor::Right:
        anchoredPosition.x = parentSize.x - (m_size.x * m_scale.x) + m_position.x;
        anchoredPosition.y = parentSize.y / 2.0f - (m_size.y * m_scale.y) / 2.0f + m_position.y;
        break;

    case UIAnchor::BottomLeft:
        anchoredPosition.y = parentSize.y - (m_size.y * m_scale.y) + m_position.y;
        break;

    case UIAnchor::Bottom:
        anchoredPosition.x = parentSize.x / 2.0f - (m_size.x * m_scale.x) / 2.0f + m_position.x;
        anchoredPosition.y = parentSize.y - (m_size.y * m_scale.y) + m_position.y;
        break;

    case UIAnchor::BottomRight:
        anchoredPosition.x = parentSize.x - (m_size.x * m_scale.x) + m_position.x;
        anchoredPosition.y = parentSize.y - (m_size.y * m_scale.y) + m_position.y;
        break;
    }

    anchoredPosition += m_anchorOffset;

    m_position = anchoredPosition;

    updateChildrenPositions();
}

void UIElement::updateChildrenPositions() {
    for (auto& child : m_children) {
        if (child) {
            child->recalculatePosition();
        }
    }
}
