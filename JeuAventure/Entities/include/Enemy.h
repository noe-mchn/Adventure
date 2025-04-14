#pragma once

#include "Entity.h"
#include <vector>

enum class EnemyType {
    Basic,
    Flying,
    Charging,
    Ranged,
    Boss
};

enum class EnemyBehavior {
    Patrol,
    Chase,
    Attack,
    Flee,
    Idle,
    Wander
};

class Enemy : public Entity {
private:
    EnemyType m_enemyType;
    EnemyBehavior m_behavior;
    int m_damage;
    int m_scoreValue;
    float m_detectionRange;
    float m_attackRange;
    bool m_isAggressive;
    Entity* m_target;

    std::vector<sf::Vector2f> m_patrolPoints;
    int m_currentPatrolPoint;
    float m_waitTime;
    float m_waitTimer;
    bool m_isWaiting;

    float m_attackCooldown;
    float m_attackTimer;
    bool m_canAttack;
    float m_chargeSpeed;

    float m_decisionInterval;
    float m_decisionTimer;

public:
    Enemy(EnemyType type = EnemyType::Basic);

    void update(float dt) override;
    void initialize() override;

    void setEnemyType(EnemyType type);
    EnemyType getEnemyType() const;

    void setBehavior(EnemyBehavior behavior);
    EnemyBehavior getBehavior() const;

    void setDamage(int damage);
    int getDamage() const;

    void setScoreValue(int value);
    int getScoreValue() const;

    void setDetectionRange(float range);
    float getDetectionRange() const;

    void setAttackRange(float range);
    float getAttackRange() const;

    void setAggressive(bool aggressive);
    bool isAggressive() const;

    void setTarget(Entity* target);
    Entity* getTarget() const;

    void addPatrolPoint(const sf::Vector2f& point);
    void clearPatrolPoints();
    void setWaitTime(float time);
    float getWaitTime() const;

    void performAttack();
    void setAttackCooldown(float cooldown);
    float getAttackCooldown() const;

    void onCollisionEnter(Collider* other) override;
    void onDeath() override;

private:
    void updatePatrolBehavior(float dt);
    void updateChaseBehavior(float dt);
    void updateAttackBehavior(float dt);
    void updateFleeBehavior(float dt);
    void updateIdleBehavior(float dt);
    void updateWanderBehavior(float dt);

    void checkForTarget(float dt);
    void moveTowards(const sf::Vector2f& target, float speed);
    void moveAway(const sf::Vector2f& target, float speed);
    float distanceTo(const sf::Vector2f& point) const;
    void updateDecision(float dt);
    void makeDecision();

    void updateAnimationState();
    void loadAnimations();
};