#include "Hitbox.h"
#include "DamageSystem.h"
#include "CollisionManager.h"
#include <algorithm>
#include <iostream>

class Entity {
public:
    virtual sf::Vector2f getPosition() const { return sf::Vector2f(0, 0); }
    virtual void setPosition(const sf::Vector2f& position) {}
};

Hitbox::Hitbox(Entity* owner, const sf::Vector2f& size, const sf::Vector2f& offset)
    : m_owner(owner),
    m_collider(owner, size, offset),
    m_damage(10.0f),
    m_knockbackForce(200.0f),
    m_knockbackDirection(1.0f, -0.5f),
    m_activeTime(0.2f),
    m_currentLifetime(0.0f),
    m_isActive(false),
    m_attackType("default")
{
    m_collider.setIsTrigger(true);
    m_collider.setTag("hitbox");
    m_collider.setCollisionLayer(static_cast<int>(CollisionLayer::Projectile));
    m_collider.setCollisionMask(static_cast<int>(CollisionLayer::Player) |
        static_cast<int>(CollisionLayer::Enemy));

    m_collider.setCollisionCallback([this](Collider* other, bool isEnter) {
        if (!isEnter || !m_isActive) return;

        Entity* target = other->getOwner();
        if (!target || target == m_owner || hasHitEntity(target)) return;

        DamageInfo damageInfo;
        damageInfo.amount = m_damage;
        damageInfo.source = m_owner;
        damageInfo.knockbackForce = m_knockbackForce;
        damageInfo.knockbackDirection = m_knockbackDirection;
        damageInfo.type = m_attackType;

        if (m_onHitCallback) {
            m_onHitCallback(target, damageInfo);
        }

        DamageSystem::getInstance()->applyDamage(target, damageInfo);
        addHitEntity(target);
        });
}

Hitbox::~Hitbox() {
    if (m_isActive) {
        CollisionManager::getInstance()->unregisterCollider(&m_collider);
    }
}

void Hitbox::setDamage(float damage) {
    m_damage = damage;
}

float Hitbox::getDamage() const {
    return m_damage;
}

void Hitbox::setKnockback(float force, const sf::Vector2f& direction) {
    m_knockbackForce = force;

    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        m_knockbackDirection = sf::Vector2f(direction.x / length, direction.y / length);
    }
    else {
        m_knockbackDirection = sf::Vector2f(1.0f, 0.0f);
    }
}

float Hitbox::getKnockbackForce() const {
    return m_knockbackForce;
}

const sf::Vector2f& Hitbox::getKnockbackDirection() const {
    return m_knockbackDirection;
}

void Hitbox::setActiveTime(float time) {
    m_activeTime = time;
}

float Hitbox::getActiveTime() const {
    return m_activeTime;
}

void Hitbox::setAttackType(const std::string& type) {
    m_attackType = type;
}

const std::string& Hitbox::getAttackType() const {
    return m_attackType;
}

Entity* Hitbox::getOwner() const {
    return m_owner;
}

BoxCollider& Hitbox::getCollider() {
    return m_collider;
}

void Hitbox::activate() {
    if (m_isActive) return;

    m_isActive = true;
    m_currentLifetime = 0.0f;
    clearHitEntities();

    CollisionManager::getInstance()->registerCollider(&m_collider);
}

void Hitbox::deactivate() {
    if (!m_isActive) return;

    m_isActive = false;
    CollisionManager::getInstance()->unregisterCollider(&m_collider);
}

bool Hitbox::isActive() const {
    return m_isActive;
}

void Hitbox::setOnHitCallback(const HitCallback& callback) {
    m_onHitCallback = callback;
}

void Hitbox::update(float dt) {
    if (!m_isActive) return;

    m_currentLifetime += dt;

    if (m_currentLifetime >= m_activeTime) {
        deactivate();
    }
}

bool Hitbox::hasHitEntity(Entity* entity) const {
    return std::find(m_hitEntities.begin(), m_hitEntities.end(), entity) != m_hitEntities.end();
}

void Hitbox::addHitEntity(Entity* entity) {
    if (!hasHitEntity(entity)) {
        m_hitEntities.push_back(entity);
    }
}

void Hitbox::clearHitEntities() {
    m_hitEntities.clear();
}

void Hitbox::debugDraw(sf::RenderWindow& window) {
    if (m_isActive) {
        m_collider.debugDraw(window);
    }
}


