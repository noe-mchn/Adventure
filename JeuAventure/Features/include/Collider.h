#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <memory>

enum class ColliderType {
    Box,
    Circle
};

enum class CollisionLayer {
    Player = 1,
    Enemy = 2,
    Platform = 4,
    Projectile = 8,
    Trigger = 16
};

inline int operator&(CollisionLayer a, CollisionLayer b) {
    return static_cast<int>(a) & static_cast<int>(b);
}

class Entity;

class Collider {
protected:
    Entity* m_owner;
    ColliderType m_type;
    sf::Vector2f m_offset;
    bool m_isTrigger;
    bool m_isEnabled;
    int m_collisionMask;
    int m_collisionLayer;
    std::string m_tag;

    using CollisionCallback = std::function<void(Collider* other, bool isEnter)>;
    CollisionCallback m_collisionCallback;

public:
    Collider(Entity* owner, ColliderType type, const sf::Vector2f& offset = sf::Vector2f(0, 0));
    virtual ~Collider() = default;

    void setOffset(const sf::Vector2f& offset);
    sf::Vector2f getOffset() const;

    void setIsTrigger(bool isTrigger);
    bool isTrigger() const;

    void setEnabled(bool enabled);
    bool isEnabled() const;

    void setCollisionMask(int mask);
    int getCollisionMask() const;

    void setCollisionLayer(int layer);
    int getCollisionLayer() const;

    void setTag(const std::string& tag);
    const std::string& getTag() const;

    ColliderType getType() const;
    Entity* getOwner() const;

    void setCollisionCallback(const CollisionCallback& callback);
    void onCollision(Collider* other, bool isEnter);
    bool shouldCollideWith(int layer) const;

    virtual sf::FloatRect getBounds() const = 0;
    virtual bool checkCollision(const Collider* other) const = 0;
    virtual void debugDraw(sf::RenderWindow& window) const = 0;
};

class BoxCollider : public Collider {
private:
    sf::Vector2f m_size;

public:
    BoxCollider(Entity* owner, const sf::Vector2f& size, const sf::Vector2f& offset = sf::Vector2f(0, 0));

    void setSize(const sf::Vector2f& size);
    sf::Vector2f getSize() const;

    sf::FloatRect getBounds() const override;
    bool checkCollision(const Collider* other) const override;
    void debugDraw(sf::RenderWindow& window) const override;
};

class CircleCollider : public Collider {
private:
    float m_radius;

public:
    CircleCollider(Entity* owner, float radius, const sf::Vector2f& offset = sf::Vector2f(0, 0));

    void setRadius(float radius);
    float getRadius() const;

    sf::FloatRect getBounds() const override;
    bool checkCollision(const Collider* other) const override;
    void debugDraw(sf::RenderWindow& window) const override;
};
