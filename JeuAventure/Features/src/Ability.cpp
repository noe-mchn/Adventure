#include "Ability.h"
#include "CombatManager.h"
#include "EventSystem.h"
#include <iostream>
#include "PhysicsEngine.h"

class Entity {
public:
    virtual sf::Vector2f getPosition() const { return sf::Vector2f(0, 0); }
    virtual void setPosition(const sf::Vector2f& position) {}
    virtual sf::Vector2f getFacingDirection() const { return sf::Vector2f(1, 0); }
    virtual PhysicsBody* getPhysicsBody() const { return nullptr; }
    virtual void playAnimation(const std::string& name, float speed = 1.0f) {}
    virtual void playSound(const std::string& name, float volume = 1.0f) {}
};

Ability::Ability(const std::string& name, float cooldown)
    : m_name(name),
    m_description(""),
    m_cooldown(cooldown),
    m_manaCost(0.0f),
    m_type("default"),
    m_isPassive(false),
    m_animationName(""),
    m_animationSpeed(1.0f),
    m_soundName(""),
    m_soundVolume(1.0f)
{
}

const std::string& Ability::getName() const {
    return m_name;
}

void Ability::setName(const std::string& name) {
    m_name = name;
}

const std::string& Ability::getDescription() const {
    return m_description;
}

void Ability::setDescription(const std::string& description) {
    m_description = description;
}

float Ability::getCooldown() const {
    return m_cooldown;
}

void Ability::setCooldown(float cooldown) {
    m_cooldown = cooldown;
}

float Ability::getManaCost() const {
    return m_manaCost;
}

void Ability::setManaCost(float cost) {
    m_manaCost = cost;
}

const std::string& Ability::getType() const {
    return m_type;
}

void Ability::setType(const std::string& type) {
    m_type = type;
}

bool Ability::isPassive() const {
    return m_isPassive;
}

void Ability::setIsPassive(bool passive) {
    m_isPassive = passive;
}

const std::string& Ability::getAnimationName() const {
    return m_animationName;
}

void Ability::setAnimationName(const std::string& name) {
    m_animationName = name;
}

float Ability::getAnimationSpeed() const {
    return m_animationSpeed;
}

void Ability::setAnimationSpeed(float speed) {
    m_animationSpeed = speed;
}

const std::string& Ability::getSoundName() const {
    return m_soundName;
}

void Ability::setSoundName(const std::string& name) {
    m_soundName = name;
}

float Ability::getSoundVolume() const {
    return m_soundVolume;
}

void Ability::setSoundVolume(float volume) {
    m_soundVolume = volume;
}

MeleeAttackAbility::MeleeAttackAbility(const std::string& name,
    const sf::Vector2f& hitboxSize,
    float damage,
    float cooldown)
    : Ability(name, cooldown),
    m_hitboxSize(hitboxSize),
    m_hitboxOffset(hitboxSize.x / 2.0f, 0.0f),
    m_damage(damage),
    m_knockbackForce(200.0f)
{
    m_type = "melee";
    m_animationName = "attack";
    m_soundName = "melee_swing";
}

const sf::Vector2f& MeleeAttackAbility::getHitboxSize() const {
    return m_hitboxSize;
}

void MeleeAttackAbility::setHitboxSize(const sf::Vector2f& size) {
    m_hitboxSize = size;
}

const sf::Vector2f& MeleeAttackAbility::getHitboxOffset() const {
    return m_hitboxOffset;
}

void MeleeAttackAbility::setHitboxOffset(const sf::Vector2f& offset) {
    m_hitboxOffset = offset;
}

float MeleeAttackAbility::getDamage() const {
    return m_damage;
}

void MeleeAttackAbility::setDamage(float damage) {
    m_damage = damage;
}

float MeleeAttackAbility::getKnockbackForce() const {
    return m_knockbackForce;
}

void MeleeAttackAbility::setKnockbackForce(float force) {
    m_knockbackForce = force;
}

bool MeleeAttackAbility::use(Entity* user, const std::map<std::string, std::any>& params, CombatManager* combatManager) {
    if (!user || !combatManager) return false;

    sf::Vector2f facingDir = user->getFacingDirection();
    sf::Vector2f offset = sf::Vector2f(
        m_hitboxOffset.x * facingDir.x,
        m_hitboxOffset.y * facingDir.y
    );

    Hitbox* hitbox = combatManager->createHitbox(user, m_hitboxSize, offset);
    if (!hitbox) return false;

    hitbox->setDamage(m_damage);
    hitbox->setKnockback(m_knockbackForce, facingDir);
    hitbox->setAttackType(m_type);
    hitbox->setActiveTime(0.2f);
    hitbox->activate();

    if (!m_animationName.empty()) {
        user->playAnimation(m_animationName, m_animationSpeed);
    }

    if (!m_soundName.empty()) {
        user->playSound(m_soundName, m_soundVolume);
    }

    return true;
}

std::unique_ptr<Ability> MeleeAttackAbility::clone() const {
    return std::make_unique<MeleeAttackAbility>(*this);
}

RangedAttackAbility::RangedAttackAbility(const std::string& name,
    const std::string& projectileType,
    float damage,
    float cooldown)
    : Ability(name, cooldown),
    m_projectileType(projectileType),
    m_projectileSpeed(500.0f),
    m_damage(damage),
    m_range(1000.0f)
{
    m_type = "ranged";
    m_animationName = "shoot";
    m_soundName = "projectile_launch";
}

const std::string& RangedAttackAbility::getProjectileType() const {
    return m_projectileType;
}

void RangedAttackAbility::setProjectileType(const std::string& type) {
    m_projectileType = type;
}

float RangedAttackAbility::getProjectileSpeed() const {
    return m_projectileSpeed;
}

void RangedAttackAbility::setProjectileSpeed(float speed) {
    m_projectileSpeed = speed;
}

float RangedAttackAbility::getDamage() const {
    return m_damage;
}

void RangedAttackAbility::setDamage(float damage) {
    m_damage = damage;
}

float RangedAttackAbility::getRange() const {
    return m_range;
}

void RangedAttackAbility::setRange(float range) {
    m_range = range;
}

bool RangedAttackAbility::use(Entity* user, const std::map<std::string, std::any>& params, CombatManager* combatManager) {
    if (!user) return false;

    sf::Vector2f facingDir = user->getFacingDirection();
    sf::Vector2f userPos = user->getPosition();

    EventSystem::getInstance()->triggerEvent("CreateProjectile", {
        {"owner", user},
        {"position", userPos},
        {"direction", facingDir},
        {"type", m_projectileType},
        {"speed", m_projectileSpeed},
        {"damage", m_damage},
        {"range", m_range}
        });

    if (!m_animationName.empty()) {
        user->playAnimation(m_animationName, m_animationSpeed);
    }

    if (!m_soundName.empty()) {
        user->playSound(m_soundName, m_soundVolume);
    }

    return true;
}

std::unique_ptr<Ability> RangedAttackAbility::clone() const {
    return std::make_unique<RangedAttackAbility>(*this);
}

DashAbility::DashAbility(const std::string& name,
    float distance,
    float speed,
    float cooldown)
    : Ability(name, cooldown),
    m_distance(distance),
    m_speed(speed),
    m_damagesEnemies(false),
    m_damage(0.0f)
{
    m_type = "movement";
    m_animationName = "dash";
    m_soundName = "dash";
}

float DashAbility::getDistance() const {
    return m_distance;
}

void DashAbility::setDistance(float distance) {
    m_distance = distance;
}

float DashAbility::getSpeed() const {
    return m_speed;
}

void DashAbility::setSpeed(float speed) {
    m_speed = speed;
}

bool DashAbility::damagesEnemies() const {
    return m_damagesEnemies;
}

void DashAbility::setDamagesEnemies(bool damages) {
    m_damagesEnemies = damages;
}

float DashAbility::getDamage() const {
    return m_damage;
}

void DashAbility::setDamage(float damage) {
    m_damage = damage;
}

bool DashAbility::use(Entity* user, const std::map<std::string, std::any>& params, CombatManager* combatManager) {
    if (!user) return false;

    sf::Vector2f facingDir = user->getFacingDirection();
    PhysicsBody* body = user->getPhysicsBody();
    if (!body) return false;

    sf::Vector2f dashVelocity = facingDir * m_speed;
    body->setVelocity(dashVelocity);

    EventSystem::getInstance()->triggerEvent("DashStarted", {
        {"entity", user},
        {"direction", facingDir},
        {"speed", m_speed},
        {"distance", m_distance}
        });

    if (m_damagesEnemies && m_damage > 0.0f && combatManager) {
        Hitbox* hitbox = combatManager->createHitbox(user, sf::Vector2f(40, 80), sf::Vector2f(0, 0));
        if (hitbox) {
            hitbox->setDamage(m_damage);
            hitbox->setKnockback(m_speed * 0.5f, facingDir);
            hitbox->setAttackType("dash");
            hitbox->setActiveTime(m_distance / m_speed);
            hitbox->activate();
        }
    }

    if (!m_animationName.empty()) {
        user->playAnimation(m_animationName, m_animationSpeed);
    }

    if (!m_soundName.empty()) {
        user->playSound(m_soundName, m_soundVolume);
    }

    return true;
}

std::unique_ptr<Ability> DashAbility::clone() const {
    return std::make_unique<DashAbility>(*this);
}
