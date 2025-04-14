#include "DamageSystem.h"
#include "PhysicsEngine.h"
#include "EventSystem.h"
#include <iostream>
#include <cmath>

class Entity {
public:
    virtual sf::Vector2f getPosition() const { return sf::Vector2f(0, 0); }
    virtual bool takeDamage(const DamageInfo& damageInfo) { return false; }
    virtual PhysicsBody* getPhysicsBody() const { return nullptr; }
};

DamageSystem* DamageSystem::s_instance = nullptr;

DamageSystem::DamageSystem()
    : m_showDamageNumbers(true)
{
    if (!m_font.loadFromFile("Assets/Fonts/damage_font.ttf")) {
        std::cerr << "Failed to load damage number font!" << std::endl;
    }
}

DamageSystem* DamageSystem::getInstance() {
    if (s_instance == nullptr) {
        s_instance = new DamageSystem();
    }
    return s_instance;
}

void DamageSystem::cleanup() {
    if (s_instance != nullptr) {
        delete s_instance;
        s_instance = nullptr;
    }
}

bool DamageSystem::applyDamage(Entity* target, const DamageInfo& damageInfo) {
    if (!target) return false;

    if (isInvincible(target)) {
        return false;
    }

    DamageModifier modifier = getDamageModifier(target, damageInfo.type);
    float finalDamage = (damageInfo.amount * modifier.multiplier) + modifier.flatBonus;

    if (finalDamage <= 0) {
        return false;
    }

    DamageInfo modifiedInfo = damageInfo;
    modifiedInfo.amount = finalDamage;

    bool damageApplied = target->takeDamage(modifiedInfo);

    if (damageApplied) {
        PhysicsBody* body = target->getPhysicsBody();
        if (body && damageInfo.knockbackForce > 0) {
            sf::Vector2f knockback = damageInfo.knockbackDirection * damageInfo.knockbackForce;
            body->applyImpulse(knockback);
        }

        if (m_showDamageNumbers) {
            DamageNumber number;
            number.text.setFont(m_font);
            number.text.setString(std::to_string(static_cast<int>(finalDamage)));
            number.text.setCharacterSize(16);
            number.text.setFillColor(sf::Color::Red);
            number.text.setOutlineColor(sf::Color::Black);
            number.text.setOutlineThickness(1.0f);

            number.position = target->getPosition() + sf::Vector2f(0, -20);
            number.velocity = sf::Vector2f(
                (std::rand() % 100 - 50) * 0.5f,
                -100.0f
            );
            number.lifetime = 1.0f;
            number.alpha = 255.0f;

            m_damageNumbers.push_back(number);
        }

        EventSystem::getInstance()->triggerEvent("EntityDamaged", {
            {"target", target},
            {"damage", finalDamage},
            {"damageType", damageInfo.type},
            {"source", damageInfo.source}
            });
    }

    return damageApplied;
}

void DamageSystem::setDamageModifier(Entity* entity, const std::string& damageType, const DamageModifier& modifier) {
    if (!entity) return;

    m_damageModifiers[entity][damageType] = modifier;
}

DamageModifier DamageSystem::getDamageModifier(Entity* entity, const std::string& damageType) const {
    if (!entity) return DamageModifier();

    auto entityIt = m_damageModifiers.find(entity);
    if (entityIt != m_damageModifiers.end()) {
        auto typeIt = entityIt->second.find(damageType);
        if (typeIt != entityIt->second.end()) {
            return typeIt->second;
        }

        typeIt = entityIt->second.find("default");
        if (typeIt != entityIt->second.end()) {
            return typeIt->second;
        }
    }

    return DamageModifier();
}

void DamageSystem::clearDamageModifiers(Entity* entity) {
    if (!entity) return;

    auto it = m_damageModifiers.find(entity);
    if (it != m_damageModifiers.end()) {
        m_damageModifiers.erase(it);
    }
}

void DamageSystem::setInvincible(Entity* entity, float duration) {
    if (!entity) return;

    m_invincibilityTimers[entity] = duration;
}

bool DamageSystem::isInvincible(Entity* entity) const {
    if (!entity) return false;

    auto it = m_invincibilityTimers.find(entity);
    return it != m_invincibilityTimers.end() && it->second > 0;
}

void DamageSystem::setShowDamageNumbers(bool show) {
    m_showDamageNumbers = show;
}

bool DamageSystem::showDamageNumbers() const {
    return m_showDamageNumbers;
}

void DamageSystem::update(float dt) {
    auto invIt = m_invincibilityTimers.begin();
    while (invIt != m_invincibilityTimers.end()) {
        invIt->second -= dt;
        if (invIt->second <= 0) {
            invIt = m_invincibilityTimers.erase(invIt);
        }
        else {
            ++invIt;
        }
    }

    auto numIt = m_damageNumbers.begin();
    while (numIt != m_damageNumbers.end()) {
        numIt->position += numIt->velocity * dt;
        numIt->velocity.y += 200.0f * dt;
        numIt->lifetime -= dt;
        numIt->alpha = 255.0f * (numIt->lifetime / 1.0f);

        if (numIt->lifetime <= 0) {
            numIt = m_damageNumbers.erase(numIt);
        }
        else {
            numIt->text.setPosition(numIt->position);

            sf::Color color = numIt->text.getFillColor();
            color.a = static_cast<sf::Uint8>(numIt->alpha);
            numIt->text.setFillColor(color);

            color = numIt->text.getOutlineColor();
            color.a = static_cast<sf::Uint8>(numIt->alpha);
            numIt->text.setOutlineColor(color);

            ++numIt;
        }
    }
}

void DamageSystem::render(sf::RenderWindow& window) {
    for (const auto& number : m_damageNumbers) {
        window.draw(number.text);
    }
}

