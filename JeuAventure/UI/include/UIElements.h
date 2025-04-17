#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <vector>
#include <memory>

enum class UIAnchor {
    TopLeft,
    Top,
    TopRight,
    Left,
    Center,
    Right,
    BottomLeft,
    Bottom,
    BottomRight
};

enum class UIScaleMode {
    None,
    StretchToFit,
    PreserveAspect,
    ScaleWithScreen
};

class UIElement {
protected:
    std::string m_id;
    bool m_visible;
    bool m_active;
    bool m_interactive;

    sf::Vector2f m_position;
    sf::Vector2f m_size;
    sf::Vector2f m_scale;
    float m_rotation;

    UIElement* m_parent;
    std::vector<std::unique_ptr<UIElement>> m_children;

    UIAnchor m_anchor;
    UIScaleMode m_scaleMode;
    sf::Vector2f m_anchorOffset;

    sf::Color m_color;
    float m_opacity;

    float m_animationSpeed;

    std::function<void(UIElement*)> m_onClickCallback;
    std::function<void(UIElement*)> m_onHoverCallback;
    std::function<void(UIElement*, bool)> m_onFocusCallback;

    void updateChildrenPositions();

public:
    UIElement();
    UIElement(const std::string& id, const sf::Vector2f& position, const sf::Vector2f& size);
    virtual ~UIElement();

    virtual void update(float dt);
    virtual void render(sf::RenderTarget& target);

    virtual bool handleEvent(const sf::Event& event);

    virtual void addChild(std::unique_ptr<UIElement> child);
    virtual UIElement* getChild(const std::string& id);
    virtual void removeChild(const std::string& id);
    virtual void clearChildren();

    virtual void setPosition(const sf::Vector2f& position);
    virtual sf::Vector2f getPosition() const;
    virtual sf::Vector2f getGlobalPosition() const;

    virtual void setSize(const sf::Vector2f& size);
    virtual sf::Vector2f getSize() const;

    virtual void setScale(const sf::Vector2f& scale);
    virtual sf::Vector2f getScale() const;

    virtual void setRotation(float rotation);
    virtual float getRotation() const;

    virtual void setAnchor(UIAnchor anchor);
    virtual UIAnchor getAnchor() const;

    virtual void setScaleMode(UIScaleMode mode);
    virtual UIScaleMode getScaleMode() const;

    virtual void setAnchorOffset(const sf::Vector2f& offset);
    virtual sf::Vector2f getAnchorOffset() const;

    virtual void setColor(const sf::Color& color);
    virtual sf::Color getColor() const;

    virtual void setOpacity(float opacity);
    virtual float getOpacity() const;

    virtual void setVisible(bool visible);
    virtual bool isVisible() const;

    virtual void setActive(bool active);
    virtual bool isActive() const;

    virtual void setInteractive(bool interactive);
    virtual bool isInteractive() const;

    virtual void setID(const std::string& id);
    virtual const std::string& getID() const;

    virtual void setParent(UIElement* parent);
    virtual UIElement* getParent() const;

    virtual void setOnClickCallback(const std::function<void(UIElement*)>& callback);
    virtual void setOnHoverCallback(const std::function<void(UIElement*)>& callback);
    virtual void setOnFocusCallback(const std::function<void(UIElement*, bool)>& callback);

    virtual void focus();
    virtual void unfocus();
    virtual void click();
    virtual void hover(bool isHovering);

    virtual bool containsPoint(const sf::Vector2f& point) const;

    virtual void recalculatePosition();
};
