#include "CombatManager.h"
#include "DamageSystem.h"
#include "Ability.h"
#include "EventSystem.h"
#include <algorithm>
#include <iostream>

CombatManager* CombatManager::s_instance = nullptr;

CombatManager::CombatManager()
    : m_debugDraw(false)
{
    m_damageSystem = DamageSystem::getInstance();
}

CombatManager* CombatManager::getInstance() {
    if (s_instance == nullptr) {
        s_instance = new CombatManager();
    }
    return s_instance;
}

void CombatManager::cleanup() {
    if (s_instance != nullptr) {
        delete s_instance;
        s_instance = nullptr;
    }
}

Hitbox* CombatManager::createHitbox(Entity* owner, const sf::Vector2f& size, const sf::Vector2f& offset) {
    if (!owner) return nullptr;

    auto hitbox = std::make_unique<Hitbox>(owner, size, offset);
    Hitbox* hitboxPtr = hitbox.get();
    m_hitboxes.push_back(std::move(hitbox));
    return hitboxPtr;
}

void CombatManager::removeHitboxesForEntity(Entity* entity) {
    if (!entity) return;

    auto it = std::remove_if(m_hitboxes.begin(), m_hitboxes.end(),
        [entity](const std::unique_ptr<Hitbox>& hitbox) {
            return hitbox->getOwner() == entity;
        });

    m_hitboxes.erase(it, m_hitboxes.end());
}

void CombatManager::registerAbility(const std::string& name, std::unique_ptr<Ability> ability) {
    if (!ability) return;

    m_abilityPrototypes[name] = std::move(ability);
}

void CombatManager::addAbilityToEntity(Entity* entity, const std::string& abilityName) {
    if (!entity) return;

    auto abilityIt = m_abilityPrototypes.find(abilityName);
    if (abilityIt == m_abilityPrototypes.end()) {
        std::cerr << "Ability not found: " << abilityName << std::endl;
        return;
    }

    auto& abilities = m_entityAbilities[entity];
    if (std::find(abilities.begin(), abilities.end(), abilityName) == abilities.end()) {
        abilities.push_back(abilityName);
    }
}

void CombatManager::removeAbilityFromEntity(Entity* entity, const std::string& abilityName) {
    if (!entity) return;

    auto entityIt = m_entityAbilities.find(entity);
    if (entityIt != m_entityAbilities.end()) {
        auto& abilities = entityIt->second;
        auto abilityIt = std::find(abilities.begin(), abilities.end(), abilityName);
        if (abilityIt != abilities.end()) {
            abilities.erase(abilityIt);
        }
    }

    auto cooldownIt = m_cooldowns.find(entity);
    if (cooldownIt != m_cooldowns.end()) {
        cooldownIt->second.erase(abilityName);
    }
}

void CombatManager::clearEntityAbilities(Entity* entity) {
    if (!entity) return;

    m_entityAbilities.erase(entity);
    m_cooldowns.erase(entity);
}

bool CombatManager::useAbility(Entity* entity, const std::string& abilityName, const std::map<std::string, std::any>& params) {
    if (!entity) return false;

    auto entityIt = m_entityAbilities.find(entity);
    if (entityIt == m_entityAbilities.end()) {
        return false;
    }

    auto& abilities = entityIt->second;
    if (std::find(abilities.begin(), abilities.end(), abilityName) == abilities.end()) {
        return false;
    }

    if (isOnCooldown(entity, abilityName)) {
        return false;
    }

    auto abilityIt = m_abilityPrototypes.find(abilityName);
    if (abilityIt == m_abilityPrototypes.end()) {
        return false;
    }

    bool success = abilityIt->second->use(entity, params, this);

    if (success) {
        setCooldown(entity, abilityName, abilityIt->second->getCooldown());

        EventSystem::getInstance()->triggerEvent("AbilityUsed", {
            {"entity", entity},
            {"abilityName", abilityName}
            });
    }

    return success;
}

void CombatManager::setCooldown(Entity* entity, const std::string& abilityName, float cooldown) {
    if (!entity) return;

    m_cooldowns[entity][abilityName] = cooldown;
}

float CombatManager::getCooldown(Entity* entity, const std::string& abilityName) const {
    if (!entity) return 0.0f;

    auto entityIt = m_cooldowns.find(entity);
    if (entityIt != m_cooldowns.end()) {
        auto abilityIt = entityIt->second.find(abilityName);
        if (abilityIt != entityIt->second.end()) {
            return abilityIt->second;
        }
    }

    return 0.0f;
}

bool CombatManager::isOnCooldown(Entity* entity, const std::string& abilityName) const {
    return getCooldown(entity, abilityName) > 0.0f;
}

void CombatManager::update(float dt) {
    auto hitboxIt = m_hitboxes.begin();
    while (hitboxIt != m_hitboxes.end()) {
        (*hitboxIt)->update(dt);

        if (!(*hitboxIt)->isActive()) {
            hitboxIt = m_hitboxes.erase(hitboxIt);
        }
        else {
            ++hitboxIt;
        }
    }

    for (auto& [entity, cooldowns] : m_cooldowns) {
        for (auto& [ability, remaining] : cooldowns) {
            if (remaining > 0.0f) {
                remaining -= dt;
                if (remaining < 0.0f) {
                    remaining = 0.0f;

                    EventSystem::getInstance()->triggerEvent("AbilityCooldownFinished", {
                        {"entity", entity},
                        {"abilityName", ability}
                        });
                }
            }
        }
    }
}

void CombatManager::setDebugDraw(bool debug) {
    m_debugDraw = debug;
}

bool CombatManager::isDebugDrawEnabled() const {
    return m_debugDraw;
}

void CombatManager::debugDraw(sf::RenderWindow& window) {
    if (!m_debugDraw) return;

    for (const auto& hitbox : m_hitboxes) {
        hitbox->debugDraw(window);
    }
}

