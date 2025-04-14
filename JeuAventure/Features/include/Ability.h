#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include <any>
#include <functional>
#include <memory>

class Entity;
class Hitbox;
class CombatManager;

class Ability {
protected:
    std::string m_name;
    std::string m_description;
    float m_cooldown;
    float m_manaCost;
    std::string m_type;
    bool m_isPassive;

    std::string m_animationName;
    float m_animationSpeed;

    std::string m_soundName;
    float m_soundVolume;

public:
    Ability(const std::string& name, float cooldown = 0.0f);
    virtual ~Ability() = default;

    const std::string& getName() const;
    void setName(const std::string& name);

    const std::string& getDescription() const;
    void setDescription(const std::string& description);

    float getCooldown() const;
    void setCooldown(float cooldown);

    float getManaCost() const;
    void setManaCost(float cost);

    const std::string& getType() const;
    void setType(const std::string& type);

    bool isPassive() const;
    void setIsPassive(bool passive);

    const std::string& getAnimationName() const;
    void setAnimationName(const std::string& name);

    float getAnimationSpeed() const;
    void setAnimationSpeed(float speed);

    const std::string& getSoundName() const;
    void setSoundName(const std::string& name);

    float getSoundVolume() const;
    void setSoundVolume(float volume);

    virtual bool use(Entity* user, const std::map<std::string, std::any>& params, CombatManager* combatManager) = 0;
    virtual std::unique_ptr<Ability> clone() const = 0;
};

class MeleeAttackAbility : public Ability {
private:
    sf::Vector2f m_hitboxSize;
    sf::Vector2f m_hitboxOffset;
    float m_damage;
    float m_knockbackForce;

public:
    MeleeAttackAbility(const std::string& name,
        const sf::Vector2f& hitboxSize,
        float damage,
        float cooldown = 0.5f);

    const sf::Vector2f& getHitboxSize() const;
    void setHitboxSize(const sf::Vector2f& size);

    const sf::Vector2f& getHitboxOffset() const;
    void setHitboxOffset(const sf::Vector2f& offset);

    float getDamage() const;
    void setDamage(float damage);

    float getKnockbackForce() const;
    void setKnockbackForce(float force);

    bool use(Entity* user, const std::map<std::string, std::any>& params, CombatManager* combatManager) override;
    std::unique_ptr<Ability> clone() const override;
};

class RangedAttackAbility : public Ability {
private:
    std::string m_projectileType;
    float m_projectileSpeed;
    float m_damage;
    float m_range;

public:
    RangedAttackAbility(const std::string& name,
        const std::string& projectileType,
        float damage,
        float cooldown = 1.0f);

    const std::string& getProjectileType() const;
    void setProjectileType(const std::string& type);

    float getProjectileSpeed() const;
    void setProjectileSpeed(float speed);

    float getDamage() const;
    void setDamage(float damage);

    float getRange() const;
    void setRange(float range);

    bool use(Entity* user, const std::map<std::string, std::any>& params, CombatManager* combatManager) override;
    std::unique_ptr<Ability> clone() const override;
};

class DashAbility : public Ability {
private:
    float m_distance;
    float m_speed;
    bool m_damagesEnemies;
    float m_damage;

public:
    DashAbility(const std::string& name,
        float distance,
        float speed,
        float cooldown = 3.0f);

    float getDistance() const;
    void setDistance(float distance);

    float getSpeed() const;
    void setSpeed(float speed);

    bool damagesEnemies() const;
    void setDamagesEnemies(bool damages);

    float getDamage() const;
    void setDamage(float damage);

    bool use(Entity* user, const std::map<std::string, std::any>& params, CombatManager* combatManager) override;
    std::unique_ptr<Ability> clone() const override;
};
