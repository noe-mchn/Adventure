#include "Collider.h"
#include <cmath>

class Entity {
public:
    virtual sf::Vector2f getPosition() const { return sf::Vector2f(0, 0); }
};

Collider::Collider(Entity* owner, ColliderType type, const sf::Vector2f& offset)
    : m_owner(owner),
    m_type(type),
    m_offset(offset),
    m_isTrigger(false),
    m_isEnabled(true),
    m_collisionMask(0xFFFFFFFF),
    m_collisionLayer(static_cast<int>(CollisionLayer::Platform)),
    m_tag("")
{
}

void Collider::setOffset(const sf::Vector2f& offset) {
    m_offset = offset;
}

sf::Vector2f Collider::getOffset() const {
    return m_offset;
}

void Collider::setIsTrigger(bool isTrigger) {
    m_isTrigger = isTrigger;
}

bool Collider::isTrigger() const {
    return m_isTrigger;
}

void Collider::setEnabled(bool enabled) {
    m_isEnabled = enabled;
}

bool Collider::isEnabled() const {
    return m_isEnabled;
}

void Collider::setCollisionMask(int mask) {
    m_collisionMask = mask;
}

int Collider::getCollisionMask() const {
    return m_collisionMask;
}

void Collider::setCollisionLayer(int layer) {
    m_collisionLayer = layer;
}

int Collider::getCollisionLayer() const {
    return m_collisionLayer;
}

void Collider::setTag(const std::string& tag) {
    m_tag = tag;
}

const std::string& Collider::getTag() const {
    return m_tag;
}

ColliderType Collider::getType() const {
    return m_type;
}

Entity* Collider::getOwner() const {
    return m_owner;
}

void Collider::setCollisionCallback(const CollisionCallback& callback) {
    m_collisionCallback = callback;
}

void Collider::onCollision(Collider* other, bool isEnter) {
    if (m_collisionCallback) {
        m_collisionCallback(other, isEnter);
    }
}

bool Collider::shouldCollideWith(int layer) const {
    return (m_collisionMask & layer) != 0;
}

BoxCollider::BoxCollider(Entity* owner, const sf::Vector2f& size, const sf::Vector2f& offset)
    : Collider(owner, ColliderType::Box, offset),
    m_size(size)
{
}

void BoxCollider::setSize(const sf::Vector2f& size) {
    m_size = size;
}

sf::Vector2f BoxCollider::getSize() const {
    return m_size;
}

sf::FloatRect BoxCollider::getBounds() const {
    sf::Vector2f ownerPos = m_owner ? m_owner->getPosition() : sf::Vector2f(0, 0);
    sf::Vector2f topLeft = ownerPos + m_offset - m_size / 2.f;
    return sf::FloatRect(topLeft.x, topLeft.y, m_size.x, m_size.y);
}

bool BoxCollider::checkCollision(const Collider* other) const {
    if (!m_isEnabled || !other->isEnabled()) {
        return false;
    }

    if (other->getType() == ColliderType::Box) {
        sf::FloatRect thisBounds = getBounds();
        sf::FloatRect otherBounds = other->getBounds();
        return thisBounds.intersects(otherBounds);
    }
    else if (other->getType() == ColliderType::Circle) {
        const CircleCollider* circle = static_cast<const CircleCollider*>(other);
        sf::FloatRect box = getBounds();
        sf::Vector2f ownerPos = other->getOwner() ? other->getOwner()->getPosition() : sf::Vector2f(0, 0);
        sf::Vector2f circleCenter = ownerPos + other->getOffset();
        float radius = circle->getRadius();

        float closestX = std::max(box.left, std::min(circleCenter.x, box.left + box.width));
        float closestY = std::max(box.top, std::min(circleCenter.y, box.top + box.height));

        float distanceX = circleCenter.x - closestX;
        float distanceY = circleCenter.y - closestY;
        float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

        return distanceSquared <= (radius * radius);
    }

    return false;
}

void BoxCollider::debugDraw(sf::RenderWindow& window) const {
    if (!m_isEnabled) return;

    sf::FloatRect bounds = getBounds();
    sf::RectangleShape rect(sf::Vector2f(bounds.width, bounds.height));
    rect.setPosition(bounds.left, bounds.top);
    rect.setFillColor(sf::Color::Transparent);

    if (m_isTrigger) {
        rect.setOutlineColor(sf::Color(0, 255, 255, 128));
    }
    else {
        rect.setOutlineColor(sf::Color(255, 0, 0, 128));
    }

    rect.setOutlineThickness(1.0f);
    window.draw(rect);
}

CircleCollider::CircleCollider(Entity* owner, float radius, const sf::Vector2f& offset)
    : Collider(owner, ColliderType::Circle, offset),
    m_radius(radius)
{
}

void CircleCollider::setRadius(float radius) {
    m_radius = radius;
}

float CircleCollider::getRadius() const {
    return m_radius;
}

sf::FloatRect CircleCollider::getBounds() const {
    sf::Vector2f ownerPos = m_owner ? m_owner->getPosition() : sf::Vector2f(0, 0);
    sf::Vector2f center = ownerPos + m_offset;
    return sf::FloatRect(center.x - m_radius, center.y - m_radius, m_radius * 2, m_radius * 2);
}

bool CircleCollider::checkCollision(const Collider* other) const {
    if (!m_isEnabled || !other->isEnabled()) {
        return false;
    }

    if (other->getType() == ColliderType::Box) {
        return other->checkCollision(this);
    }
    else if (other->getType() == ColliderType::Circle) {
        const CircleCollider* otherCircle = static_cast<const CircleCollider*>(other);
        sf::Vector2f thisPos = m_owner ? m_owner->getPosition() : sf::Vector2f(0, 0);
        sf::Vector2f otherPos = other->getOwner() ? other->getOwner()->getPosition() : sf::Vector2f(0, 0);

        sf::Vector2f thisCenter = thisPos + m_offset;
        sf::Vector2f otherCenter = otherPos + other->getOffset();

        float dx = thisCenter.x - otherCenter.x;
        float dy = thisCenter.y - otherCenter.y;
        float distanceSquared = dx * dx + dy * dy;

        float radiusSum = m_radius + otherCircle->getRadius();
        return distanceSquared <= (radiusSum * radiusSum);
    }

    return false;
}

void CircleCollider::debugDraw(sf::RenderWindow& window) const {
    if (!m_isEnabled) return;

    sf::Vector2f ownerPos = m_owner ? m_owner->getPosition() : sf::Vector2f(0, 0);
    sf::Vector2f center = ownerPos + m_offset;

    sf::CircleShape circle(m_radius);
    circle.setOrigin(m_radius, m_radius);
    circle.setPosition(center);
    circle.setFillColor(sf::Color::Transparent);

    if (m_isTrigger) {
        circle.setOutlineColor(sf::Color(0, 255, 255, 128));
    }
    else {
        circle.setOutlineColor(sf::Color(255, 0, 0, 128));
    }

    circle.setOutlineThickness(1.0f);
    window.draw(circle);
}

