#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include <string>
#include <map>
#include <memory>

class Entity;

struct DamageInfo {
    float amount;
    Entity* source;
    float knockbackForce;
    sf::Vector2f knockbackDirection;
    std::string type;

    DamageInfo()
        : amount(0.0f),
        source(nullptr),
        knockbackForce(0.0f),
        knockbackDirection(0.0f, 0.0f),
        type("default")
    {
    }
};

struct DamageModifier {
    float multiplier;
    float flatBonus;

    DamageModifier()
        : multiplier(1.0f),
        flatBonus(0.0f)
    {
    }

    DamageModifier(float mult, float bonus)
        : multiplier(mult),
        flatBonus(bonus)
    {
    }
};

class DamageSystem {
private:
    static DamageSystem* s_instance;

    std::map<Entity*, std::map<std::string, DamageModifier>> m_damageModifiers;
    std::map<Entity*, float> m_invincibilityTimers;

    struct DamageNumber {
        sf::Text text;
        sf::Vector2f position;
        sf::Vector2f velocity;
        float lifetime;
        float alpha;
    };

    std::vector<DamageNumber> m_damageNumbers;
    sf::Font m_font;
    bool m_showDamageNumbers;

    DamageSystem();

public:
    DamageSystem(const DamageSystem&) = delete;
    DamageSystem& operator=(const DamageSystem&) = delete;

    static DamageSystem* getInstance();
    static void cleanup();

    bool applyDamage(Entity* target, const DamageInfo& damageInfo);

    void setDamageModifier(Entity* entity, const std::string& damageType, const DamageModifier& modifier);
    DamageModifier getDamageModifier(Entity* entity, const std::string& damageType) const;
    void clearDamageModifiers(Entity* entity);

    void setInvincible(Entity* entity, float duration);
    bool isInvincible(Entity* entity) const;

    void setShowDamageNumbers(bool show);
    bool showDamageNumbers() const;

    void update(float dt);
    void render(sf::RenderWindow& window);
};
