#pragma once

#include <any>
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <memory>
#include <string>
#include "Hitbox.h"

class Entity;
class Ability;
class DamageSystem;

class CombatManager {
private:
    static CombatManager* s_instance;

    std::vector<std::unique_ptr<Hitbox>> m_hitboxes;
    std::map<std::string, std::unique_ptr<Ability>> m_abilityPrototypes;
    std::map<Entity*, std::vector<std::string>> m_entityAbilities;
    std::map<Entity*, std::map<std::string, float>> m_cooldowns;
    bool m_debugDraw;
    DamageSystem* m_damageSystem;

    CombatManager();

public:
    CombatManager(const CombatManager&) = delete;
    CombatManager& operator=(const CombatManager&) = delete;

    static CombatManager* getInstance();
    static void cleanup();

    Hitbox* createHitbox(Entity* owner, const sf::Vector2f& size, const sf::Vector2f& offset = sf::Vector2f(0, 0));
    void removeHitboxesForEntity(Entity* entity);

    void registerAbility(const std::string& name, std::unique_ptr<Ability> ability);
    void addAbilityToEntity(Entity* entity, const std::string& abilityName);
    void removeAbilityFromEntity(Entity* entity, const std::string& abilityName);
    void clearEntityAbilities(Entity* entity);

    bool useAbility(Entity* entity, const std::string& abilityName, const std::map<std::string, std::any>& params = {});

    void setCooldown(Entity* entity, const std::string& abilityName, float cooldown);
    float getCooldown(Entity* entity, const std::string& abilityName) const;
    bool isOnCooldown(Entity* entity, const std::string& abilityName) const;

    void update(float dt);

    void setDebugDraw(bool debug);
    bool isDebugDrawEnabled() const;
    void debugDraw(sf::RenderWindow& window);
};
