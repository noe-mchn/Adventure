#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <memory>
#include "Collider.h"

class Entity;
class DamageInfo;

class Hitbox {
private:
    Entity* m_owner;
    BoxCollider m_collider;
    float m_damage;
    float m_knockbackForce;
    sf::Vector2f m_knockbackDirection;
    float m_activeTime;
    float m_currentLifetime;
    bool m_isActive;
    std::string m_attackType;

    std::vector<Entity*> m_hitEntities;

    using HitCallback = std::function<void(Entity* target, const DamageInfo& damageInfo)>;
    HitCallback m_onHitCallback;

public:
    Hitbox(Entity* owner, const sf::Vector2f& size, const sf::Vector2f& offset = sf::Vector2f(0, 0));
    ~Hitbox();

    void setDamage(float damage);
    float getDamage() const;

    void setKnockback(float force, const sf::Vector2f& direction);
    float getKnockbackForce() const;
    const sf::Vector2f& getKnockbackDirection() const;

    void setActiveTime(float time);
    float getActiveTime() const;

    void setAttackType(const std::string& type);
    const std::string& getAttackType() const;

    Entity* getOwner() const;
    BoxCollider& getCollider();

    void activate();
    void deactivate();
    bool isActive() const;

    void setOnHitCallback(const HitCallback& callback);

    void update(float dt);

    bool hasHitEntity(Entity* entity) const;
    void addHitEntity(Entity* entity);
    void clearHitEntities();

    void debugDraw(sf::RenderWindow& window);
};
