#include "Enemy.h"
#include "CombatManager.h"
#include "EventSystem.h"
#include "RessourceManager.h"
#include "Player.h"
#include <iostream>
#include <cmath>

Enemy::Enemy(EnemyType type)
    : Entity(EntityType::Enemy),
    m_enemyType(type),
    m_behavior(EnemyBehavior::Patrol),
    m_damage(10),
    m_scoreValue(100),
    m_detectionRange(300.0f),
    m_attackRange(50.0f),
    m_isAggressive(true),
    m_target(nullptr),
    m_currentPatrolPoint(0),
    m_waitTime(1.0f),
    m_waitTimer(0.0f),
    m_isWaiting(false),
    m_attackCooldown(2.0f),
    m_attackTimer(0.0f),
    m_canAttack(true),
    m_chargeSpeed(400.0f),
    m_decisionInterval(1.0f),
    m_decisionTimer(0.0f)
{
    m_name = "Enemy";
    m_speed = 100.0f;

    switch (m_enemyType) {
    case EnemyType::Flying:
        m_speed = 150.0f;
        m_health = 30;
        m_maxHealth = 30;
        m_damage = 5;
        m_scoreValue = 150;
        break;

    case EnemyType::Charging:
        m_speed = 80.0f;
        m_chargeSpeed = 400.0f;
        m_health = 50;
        m_maxHealth = 50;
        m_damage = 20;
        m_scoreValue = 200;
        break;

    case EnemyType::Ranged:
        m_speed = 70.0f;
        m_health = 40;
        m_maxHealth = 40;
        m_attackRange = 300.0f;
        m_damage = 15;
        m_scoreValue = 250;
        break;

    case EnemyType::Boss:
        m_speed = 120.0f;
        m_health = 300;
        m_maxHealth = 300;
        m_damage = 30;
        m_scoreValue = 1000;
        break;

    case EnemyType::Basic:
    default:
        m_speed = 100.0f;
        m_health = 40;
        m_maxHealth = 40;
        m_damage = 10;
        m_scoreValue = 100;
        break;
    }
}

void Enemy::initialize() {
    Entity::initialize();

    if (m_collider) {
        m_collider->setCollisionLayer(static_cast<int>(CollisionLayer::Enemy));

        switch (m_enemyType) {
        case EnemyType::Flying:
            setSize(48.0f, 32.0f);
            break;

        case EnemyType::Charging:
            setSize(64.0f, 48.0f);
            break;

        case EnemyType::Ranged:
            setSize(40.0f, 60.0f);
            break;

        case EnemyType::Boss:
            setSize(96.0f, 96.0f);
            break;

        case EnemyType::Basic:
        default:
            setSize(40.0f, 40.0f);
            break;
        }
    }

    if (m_physicsBody) {
        PhysicsProperties props;
        props.mass = 1.0f;

        if (m_enemyType == EnemyType::Flying) {
            props.gravityScale = 0.1f;
        }
        else {
            props.gravityScale = 1.0f;
        }

        props.friction = 0.1f;
        props.restitution = 0.0f;
        m_physicsBody->setProperties(props);
    }

    loadAnimations();

    if (m_behavior == EnemyBehavior::Patrol && m_patrolPoints.empty()) {
        m_behavior = EnemyBehavior::Idle;
    }
}

void Enemy::update(float dt) {
    if (m_state == EntityState::Dead) {
        Entity::update(dt);
        return;
    }

    if (m_attackTimer > 0.0f) {
        m_attackTimer -= dt;
        if (m_attackTimer <= 0.0f) {
            m_canAttack = true;
        }
    }

    if (m_waitTimer > 0.0f) {
        m_waitTimer -= dt;
        if (m_waitTimer <= 0.0f) {
            m_isWaiting = false;
        }
    }

    checkForTarget(dt);

    updateDecision(dt);

    switch (m_behavior) {
    case EnemyBehavior::Patrol:
        updatePatrolBehavior(dt);
        break;

    case EnemyBehavior::Chase:
        updateChaseBehavior(dt);
        break;

    case EnemyBehavior::Attack:
        updateAttackBehavior(dt);
        break;

    case EnemyBehavior::Flee:
        updateFleeBehavior(dt);
        break;

    case EnemyBehavior::Idle:
        updateIdleBehavior(dt);
        break;

    case EnemyBehavior::Wander:
        updateWanderBehavior(dt);
        break;
    }

    updateAnimationState();

    Entity::update(dt);
}

void Enemy::setEnemyType(EnemyType type) {
    m_enemyType = type;
}

EnemyType Enemy::getEnemyType() const {
    return m_enemyType;
}

void Enemy::setBehavior(EnemyBehavior behavior) {
    m_behavior = behavior;
}

EnemyBehavior Enemy::getBehavior() const {
    return m_behavior;
}

void Enemy::setDamage(int damage) {
    m_damage = damage;
}

int Enemy::getDamage() const {
    return m_damage;
}

void Enemy::setScoreValue(int value) {
    m_scoreValue = value;
}

int Enemy::getScoreValue() const {
    return m_scoreValue;
}

void Enemy::setDetectionRange(float range) {
    m_detectionRange = range;
}

float Enemy::getDetectionRange() const {
    return m_detectionRange;
}

void Enemy::setAttackRange(float range) {
    m_attackRange = range;
}

float Enemy::getAttackRange() const {
    return m_attackRange;
}

void Enemy::setAggressive(bool aggressive) {
    m_isAggressive = aggressive;
}

bool Enemy::isAggressive() const {
    return m_isAggressive;
}

void Enemy::setTarget(Entity* target) {
    m_target = target;
}

Entity* Enemy::getTarget() const {
    return m_target;
}

void Enemy::addPatrolPoint(const sf::Vector2f& point) {
    m_patrolPoints.push_back(point);
}

void Enemy::clearPatrolPoints() {
    m_patrolPoints.clear();
    m_currentPatrolPoint = 0;
}

void Enemy::setWaitTime(float time) {
    m_waitTime = time;
}

float Enemy::getWaitTime() const {
    return m_waitTime;
}

void Enemy::performAttack() {
    if (!m_canAttack) return;

    m_canAttack = false;
    m_attackTimer = m_attackCooldown;

    setState(EntityState::Attacking);

    CombatManager* combatManager = CombatManager::getInstance();
    if (combatManager) {
        sf::Vector2f hitboxSize;
        sf::Vector2f offset;
        float damage = static_cast<float>(m_damage);

        switch (m_enemyType) {
        case EnemyType::Flying:
            hitboxSize = sf::Vector2f(40.0f, 20.0f);
            offset = sf::Vector2f(0.0f, 0.0f);
            break;

        case EnemyType::Charging:
            hitboxSize = sf::Vector2f(70.0f, 40.0f);
            offset = sf::Vector2f(40.0f * (m_facingRight ? 1.0f : -1.0f), 0.0f);
            break;

        case EnemyType::Ranged:
            EventSystem::getInstance()->triggerEvent("CreateProjectile", {
                {"owner", this},
                {"position", m_position},
                {"direction", sf::Vector2f(m_facingRight ? 1.0f : -1.0f, 0.0f)},
                {"type", "enemy_projectile"},
                {"speed", 300.0f},
                {"damage", damage},
                {"range", 500.0f}
                });

            playSound("enemy_shoot", 1.0f);
            return;

        case EnemyType::Boss:
            hitboxSize = sf::Vector2f(100.0f, 80.0f);
            offset = sf::Vector2f(60.0f * (m_facingRight ? 1.0f : -1.0f), 0.0f);
            break;

        case EnemyType::Basic:
        default:
            hitboxSize = sf::Vector2f(50.0f, 40.0f);
            offset = sf::Vector2f(30.0f * (m_facingRight ? 1.0f : -1.0f), 0.0f);
            break;
        }

        Hitbox* hitbox = combatManager->createHitbox(this, hitboxSize, offset);
        if (hitbox) {
            hitbox->setDamage(damage);
            hitbox->setKnockback(150.0f, sf::Vector2f(m_facingRight ? 1.0f : -1.0f, -0.2f));
            hitbox->setAttackType("enemy");
            hitbox->setActiveTime(0.2f);
            hitbox->activate();
        }
    }

    playSound("enemy_attack", 1.0f);
}

void Enemy::setAttackCooldown(float cooldown) {
    m_attackCooldown = cooldown;
}

float Enemy::getAttackCooldown() const {
    return m_attackCooldown;
}

void Enemy::onCollisionEnter(Collider* other) {
    Entity::onCollisionEnter(other);

    if (!other || !other->getOwner()) return;

    Entity* otherEntity = static_cast<Entity*>(other->getOwner());

    if (otherEntity->getType() == EntityType::Player) {
        if (m_state == EntityState::Attacking) {
            return;
        }

        if (m_behavior == EnemyBehavior::Attack &&
            (m_enemyType == EnemyType::Charging || m_enemyType == EnemyType::Flying)) {

            DamageInfo damageInfo;
            damageInfo.amount = static_cast<float>(m_damage);
            damageInfo.source = this;
            damageInfo.knockbackForce = 200.0f;
            damageInfo.knockbackDirection = sf::Vector2f(m_facingRight ? 1.0f : -1.0f, -0.2f);
            damageInfo.type = "enemy_contact";

            otherEntity->takeDamage(damageInfo);

            sf::Vector2f recoil = sf::Vector2f((m_facingRight ? -1.0f : 1.0f) * 100.0f, 0.0f);
            if (m_physicsBody) {
                m_physicsBody->applyImpulse(recoil);
            }
        }
    }
}

void Enemy::onDeath() {
    Entity::onDeath();

    EventSystem::getInstance()->triggerEvent("EnemyDied", {
        {"enemy", this},
        {"type", static_cast<int>(m_enemyType)},
        {"position", m_position},
        {"scoreValue", m_scoreValue}
        });

    const int dropChance = 30;
    if (rand() % 100 < dropChance) {
        EventSystem::getInstance()->triggerEvent("CreatePickup", {
            {"position", m_position},
            {"type", (rand() % 2 == 0) ? "health" : "coin"}
            });
    }

    playSound("enemy_death", 1.0f);
}

void Enemy::updatePatrolBehavior(float dt) {
    if (m_patrolPoints.empty() || m_isWaiting) {
        setState(EntityState::Idle);
        return;
    }

    sf::Vector2f destination = m_patrolPoints[m_currentPatrolPoint];

    moveTowards(destination, m_speed);

    if (distanceTo(destination) < 10.0f) {
        m_isWaiting = true;
        m_waitTimer = m_waitTime;
        setState(EntityState::Idle);

        m_currentPatrolPoint = (m_currentPatrolPoint + 1) % m_patrolPoints.size();
    }
}

void Enemy::updateChaseBehavior(float dt) {
    if (!m_target) {
        m_behavior = EnemyBehavior::Patrol;
        return;
    }

    moveTowards(m_target->getPosition(), m_speed);

    if (distanceTo(m_target->getPosition()) < m_attackRange) {
        m_behavior = EnemyBehavior::Attack;
    }
}

void Enemy::updateAttackBehavior(float dt) {
    if (!m_target || distanceTo(m_target->getPosition()) > m_attackRange * 1.2f) {
        m_behavior = EnemyBehavior::Chase;
        return;
    }

    setFacingRight(m_target->getPosition().x > m_position.x);

    if (m_canAttack) {
        performAttack();
    }
    else {
        if (m_enemyType == EnemyType::Charging && m_state != EntityState::Attacking) {
            moveTowards(m_target->getPosition(), m_chargeSpeed);
        }
        else if (m_enemyType == EnemyType::Ranged) {
            float distance = distanceTo(m_target->getPosition());
            if (distance < m_attackRange * 0.5f) {
                moveAway(m_target->getPosition(), m_speed * 0.7f);
            }
        }
    }
}

void Enemy::updateFleeBehavior(float dt) {
    if (!m_target) {
        m_behavior = EnemyBehavior::Patrol;
        return;
    }

    moveAway(m_target->getPosition(), m_speed);

    if (distanceTo(m_target->getPosition()) > m_detectionRange * 1.5f) {
        m_behavior = EnemyBehavior::Patrol;
    }
}

void Enemy::updateIdleBehavior(float dt) {
    setState(EntityState::Idle);
}

void Enemy::updateWanderBehavior(float dt) {
    if (m_isWaiting) {
        setState(EntityState::Idle);
        return;
    }

    sf::Vector2f moveVec = sf::Vector2f(m_facingRight ? 1.0f : -1.0f, 0.0f);

    sf::Vector2f currentVel = getVelocity();
    setVelocity(moveVec.x * m_speed, currentVel.y);

    setState(EntityState::Walking);

    if (m_decisionTimer <= 0.0f) {
        makeDecision();
    }
}

void Enemy::checkForTarget(float dt) {
    if (!m_isAggressive || m_behavior == EnemyBehavior::Attack || m_behavior == EnemyBehavior::Flee) {
        return;
    }

    Player* nearestPlayer = nullptr;
    float nearestDistance = m_detectionRange;

    if (m_target && m_target->getType() == EntityType::Player) {
        float distance = distanceTo(m_target->getPosition());

        if (distance < m_detectionRange) {
            if (m_behavior != EnemyBehavior::Chase && m_behavior != EnemyBehavior::Attack) {
                m_behavior = EnemyBehavior::Chase;

                playSound("enemy_alert", 1.0f);
            }
        }
        else if (m_behavior == EnemyBehavior::Chase) {
            m_behavior = EnemyBehavior::Patrol;
        }
    }
}

void Enemy::moveTowards(const sf::Vector2f& target, float speed) {
    sf::Vector2f direction = target - m_position;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length > 0) {
        direction /= length;
    }

    if (direction.x != 0.0f) {
        setFacingRight(direction.x > 0.0f);
    }

    if (m_enemyType == EnemyType::Flying) {
        sf::Vector2f velocity = direction * speed;
        setVelocity(velocity);
    }
    else {
        sf::Vector2f currentVel = getVelocity();
        setVelocity(direction.x * speed, currentVel.y);
    }

    if (m_state != EntityState::Jumping &&
        m_state != EntityState::Falling &&
        m_state != EntityState::Attacking &&
        m_state != EntityState::Hit) {

        setState(EntityState::Walking);
    }
}

void Enemy::moveAway(const sf::Vector2f& target, float speed) {
    sf::Vector2f direction = m_position - target;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length > 0) {
        direction /= length;
    }

    if (direction.x != 0.0f) {
        setFacingRight(direction.x > 0.0f);
    }

    if (m_enemyType == EnemyType::Flying) {
        sf::Vector2f velocity = direction * speed;
        setVelocity(velocity);
    }
    else {
        sf::Vector2f currentVel = getVelocity();
        setVelocity(direction.x * speed, currentVel.y);
    }

    setState(EntityState::Walking);
}

float Enemy::distanceTo(const sf::Vector2f& point) const {
    sf::Vector2f diff = m_position - point;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

void Enemy::updateDecision(float dt) {
    m_decisionTimer -= dt;

    if (m_decisionTimer <= 0.0f && m_behavior == EnemyBehavior::Wander) {
        makeDecision();
        m_decisionTimer = m_decisionInterval;
    }
}

void Enemy::makeDecision() {
    if (m_behavior == EnemyBehavior::Wander) {
        int decision = rand() % 10;

        if (decision < 3) {
            setFacingRight(!m_facingRight);
        }
        else if (decision < 5) {
            m_isWaiting = true;
            m_waitTimer = m_waitTime;
            setState(EntityState::Idle);
        }
    }
}

void Enemy::updateAnimationState() {
    switch (m_state) {
    case EntityState::Idle:
        playAnimation("idle");
        break;
    case EntityState::Walking:
        playAnimation("walk");
        break;
    case EntityState::Running:
        playAnimation("run");
        break;
    case EntityState::Jumping:
        playAnimation("jump");
        break;
    case EntityState::Falling:
        playAnimation("fall");
        break;
    case EntityState::Attacking:
        playAnimation("attack");
        break;
    case EntityState::Hit:
        playAnimation("hit");
        break;
    case EntityState::Dead:
        playAnimation("death");
        break;
    }
}

void Enemy::loadAnimations() {
    RessourceManager* resourceManager = RessourceManager::getInstance();

    std::string textureKey = "enemy_basic_idle";
    switch (m_enemyType) {
    case EnemyType::Flying:
        textureKey = "enemy_flying_idle";
        break;
    case EnemyType::Charging:
        textureKey = "enemy_charging_idle";
        break;
    case EnemyType::Ranged:
        textureKey = "enemy_ranged_idle";
        break;
    case EnemyType::Boss:
        textureKey = "enemy_boss_idle";
        break;
    default:
        textureKey = "enemy_basic_idle";
        break;
    }

    if (resourceManager->loadTexture(textureKey, "Assets/Textures/" + textureKey + ".png")) {
        m_texture = std::make_unique<sf::Texture>(*resourceManager->getTexture(textureKey));
        m_sprite.setTexture(*m_texture);
        m_sprite.setOrigin(m_texture->getSize().x / 2.0f, m_texture->getSize().y / 2.0f);
    }
}
