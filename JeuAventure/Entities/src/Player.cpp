#include "Player.h"
#include "CombatManager.h"
#include "InputManager.h"
#include "EventSystem.h"
#include "RessourceManager.h"
#include <iostream>

Player::Player()
    : Entity(EntityType::Player),
    m_coins(0),
    m_score(0),
    m_lives(3),
    m_dashCooldown(1.0f),
    m_dashTimer(0.0f),
    m_canDash(true),
    m_attackCooldown(0.3f),
    m_attackTimer(0.0f),
    m_canAttack(true),
    m_hasDash(true),
    m_hasDoubleJump(true),
    m_hasWallJump(true),
    m_jumpsRemaining(2),
    m_maxJumps(2),
    m_isOnWall(false),
    m_isWallSliding(false),
    m_wallSlideSpeed(100.0f),
    m_wallNormal(0.0f, 0.0f),
    m_wallJumpForce(400.0f),
    m_coyoteTime(0.1f),
    m_coyoteTimer(0.0f),
    m_jumpBufferTime(0.2f),
    m_jumpBufferTimer(0.0f),
    m_jumpBuffered(false)
{
    m_name = "Player";
    m_speed = 250.0f;
    m_jumpForce = 500.0f;
    m_size = sf::Vector2f(32.0f, 64.0f);
    m_health = 100;
    m_maxHealth = 100;

    m_actions[PlayerAction::MoveLeft] = false;
    m_actions[PlayerAction::MoveRight] = false;
    m_actions[PlayerAction::Jump] = false;
    m_actions[PlayerAction::Attack] = false;
    m_actions[PlayerAction::Dash] = false;
    m_actions[PlayerAction::Special] = false;
    m_actions[PlayerAction::Interact] = false;
}

void Player::initialize() {
    Entity::initialize();

    if (m_collider) {
        m_collider->setCollisionLayer(static_cast<int>(CollisionLayer::Player));
    }

    if (m_physicsBody) {
        PhysicsProperties props;
        props.mass = 1.0f;
        props.gravityScale = 1.0f;
        props.friction = 0.1f;
        props.restitution = 0.0f;
        m_physicsBody->setProperties(props);
    }

    loadAnimations();
    resetJumps();
}

void Player::update(float dt) {
    updateMovement(dt);
    updateJump(dt);
    updateDash(dt);
    updateAttack(dt);
    updateWallSlide(dt);

    if (m_dashTimer > 0.0f) {
        m_dashTimer -= dt;
        if (m_dashTimer <= 0.0f) {
            m_canDash = true;
        }
    }

    if (m_attackTimer > 0.0f) {
        m_attackTimer -= dt;
        if (m_attackTimer <= 0.0f) {
            m_canAttack = true;
        }
    }

    if (!m_isGrounded && m_coyoteTimer > 0.0f) {
        m_coyoteTimer -= dt;
    }

    if (m_jumpBuffered && m_jumpBufferTimer > 0.0f) {
        m_jumpBufferTimer -= dt;
        if (m_jumpBufferTimer <= 0.0f) {
            m_jumpBuffered = false;
        }
        else if (m_isGrounded) {
            jump();
            m_jumpBuffered = false;
        }
    }

    updateAnimationState();
    Entity::update(dt);
}

void Player::handleEvents(const sf::Event& event) {
}

void Player::move(float x, float y) {
    if (m_state == EntityState::Dead) return;

    sf::Vector2f moveVec(x, y);
    float length = std::sqrt(x * x + y * y);

    if (length > 1.0f) {
        moveVec.x /= length;
        moveVec.y /= length;
    }

    if (x != 0.0f) {
        setFacingRight(x > 0.0f);
    }

    sf::Vector2f currentVel = getVelocity();
    setVelocity(moveVec.x * m_speed, currentVel.y);

    if (m_state != EntityState::Jumping &&
        m_state != EntityState::Falling &&
        m_state != EntityState::Attacking &&
        m_state != EntityState::Dashing &&
        m_state != EntityState::Hit) {

        if (x != 0.0f) {
            setState(EntityState::Walking);
        }
        else {
            setState(EntityState::Idle);
        }
    }
}

void Player::jump() {
    if (m_state == EntityState::Dead || m_state == EntityState::Dashing) return;

    bool canJumpNow = false;

    if (m_isGrounded) {
        canJumpNow = true;
        m_jumpsRemaining = m_maxJumps - 1;
    }
    else if (m_coyoteTimer > 0.0f) {
        canJumpNow = true;
        m_coyoteTimer = 0.0f;
        m_jumpsRemaining = m_maxJumps - 1;
    }
    else if (m_isWallSliding && m_hasWallJump) {
        canJumpNow = true;

        sf::Vector2f jumpDir = m_wallNormal;
        jumpDir.y = -0.8f;
        float length = std::sqrt(jumpDir.x * jumpDir.x + jumpDir.y * jumpDir.y);
        if (length > 0) {
            jumpDir /= length;
        }

        if (m_physicsBody) {
            m_physicsBody->setVelocity(jumpDir * m_wallJumpForce);
        }

        setFacingRight(m_wallNormal.x > 0);
        playSound("wall_jump", 1.0f);
        return;
    }
    else if (m_jumpsRemaining > 0 && m_hasDoubleJump) {
        canJumpNow = true;
        m_jumpsRemaining--;
    }
    else {
        m_jumpBuffered = true;
        m_jumpBufferTimer = m_jumpBufferTime;
        return;
    }

    if (canJumpNow) {
        if (m_physicsBody) {
            m_physicsBody->setVelocity(sf::Vector2f(getVelocity().x, -m_jumpForce));
        }

        setState(EntityState::Jumping);
        playSound("jump", 1.0f);
    }
}

void Player::attack() {
    if (m_state == EntityState::Dead || !m_canAttack) return;

    setState(EntityState::Attacking);

    m_canAttack = false;
    m_attackTimer = m_attackCooldown;

    CombatManager* combatManager = CombatManager::getInstance();
    if (combatManager) {
        sf::Vector2f hitboxSize(50.0f, 40.0f);
        sf::Vector2f offset(40.0f * (m_facingRight ? 1.0f : -1.0f), 0.0f);

        Hitbox* hitbox = combatManager->createHitbox(this, hitboxSize, offset);
        if (hitbox) {
            hitbox->setDamage(20.0f);
            hitbox->setKnockback(200.0f, sf::Vector2f(m_facingRight ? 1.0f : -1.0f, -0.2f));
            hitbox->setAttackType("slash");
            hitbox->setActiveTime(0.2f);
            hitbox->activate();
        }
    }

    playSound("attack", 1.0f);
}

void Player::dash() {
    if (m_state == EntityState::Dead || !m_canDash || !m_hasDash) return;

    m_canDash = false;
    m_dashTimer = m_dashCooldown;

    setState(EntityState::Dashing);

    sf::Vector2f dashDir = getFacingDirection();
    if (m_physicsBody) {
        m_physicsBody->setVelocity(dashDir * 800.0f);
        setInvulnerable(true, 0.3f);
    }

    playSound("dash", 1.0f);

    CombatManager* combatManager = CombatManager::getInstance();
    if (combatManager) {
        combatManager->useAbility(this, "dash", {
            {"direction", dashDir},
            {"speed", 800.0f},
            {"distance", 200.0f}
            });
    }
}

void Player::interact() {
    sf::Vector2f interactPos = m_position + getFacingDirection() * 32.0f;

    EventSystem::getInstance()->triggerEvent("PlayerInteract", {
        {"player", this},
        {"position", interactPos}
        });
}

void Player::setAction(PlayerAction action, bool active) {
    m_actions[action] = active;
}

bool Player::isActionActive(PlayerAction action) const {
    auto it = m_actions.find(action);
    if (it != m_actions.end()) {
        return it->second;
    }
    return false;
}

void Player::setHasDash(bool hasDash) {
    m_hasDash = hasDash;
}

bool Player::hasDash() const {
    return m_hasDash;
}

void Player::setHasDoubleJump(bool hasDoubleJump) {
    m_hasDoubleJump = hasDoubleJump;
    if (hasDoubleJump) {
        m_maxJumps = 2;
    }
    else {
        m_maxJumps = 1;
    }
}

bool Player::hasDoubleJump() const {
    return m_hasDoubleJump;
}

void Player::setHasWallJump(bool hasWallJump) {
    m_hasWallJump = hasWallJump;
}

bool Player::hasWallJump() const {
    return m_hasWallJump;
}

void Player::resetJumps() {
    m_jumpsRemaining = m_maxJumps;
}

int Player::getJumpsRemaining() const {
    return m_jumpsRemaining;
}

int Player::getMaxJumps() const {
    return m_maxJumps;
}

void Player::addCoins(int amount) {
    m_coins += amount;
    EventSystem::getInstance()->triggerEvent("PlayerCoinsChanged", {
        {"player", this},
        {"coins", m_coins}
        });
}

int Player::getCoins() const {
    return m_coins;
}

void Player::addScore(int amount) {
    m_score += amount;
    EventSystem::getInstance()->triggerEvent("PlayerScoreChanged", {
        {"player", this},
        {"score", m_score}
        });
}

int Player::getScore() const {
    return m_score;
}

void Player::addLives(int amount) {
    m_lives += amount;
    EventSystem::getInstance()->triggerEvent("PlayerLivesChanged", {
        {"player", this},
        {"lives", m_lives}
        });
}

int Player::getLives() const {
    return m_lives;
}

void Player::onCollisionEnter(Collider* other) {
    Entity::onCollisionEnter(other);

    if (!other || !other->getOwner()) return;

    Entity* otherEntity = static_cast<Entity*>(other->getOwner());

    if ((other->getCollisionLayer() & static_cast<int>(CollisionLayer::Platform)) &&
        !m_isGrounded) {
        sf::FloatRect thisBounds = getBounds();
        sf::FloatRect otherBounds = other->getBounds();

        float overlapX = std::min(thisBounds.left + thisBounds.width, otherBounds.left + otherBounds.width) -
            std::max(thisBounds.left, otherBounds.left);
        float overlapY = std::min(thisBounds.top + thisBounds.height, otherBounds.top + otherBounds.height) -
            std::max(thisBounds.top, otherBounds.top);

        if (overlapX < overlapY) {
            m_isOnWall = true;

            if (m_position.x < otherBounds.left + otherBounds.width / 2.0f) {
                m_wallNormal = sf::Vector2f(-1.0f, 0.0f);
            }
            else {
                m_wallNormal = sf::Vector2f(1.0f, 0.0f);
            }

            if (getVelocity().y > 0 && m_hasWallJump) {
                m_isWallSliding = true;
            }
        }
    }

    if (otherEntity->getType() == EntityType::Pickup) {
        if (otherEntity->getName() == "Coin") {
            addCoins(1);
            addScore(100);
            playSound("coin_pickup", 1.0f);
        }
        else if (otherEntity->getName() == "Health") {
            heal(20);
            playSound("health_pickup", 1.0f);
        }

        otherEntity->setActive(false);
    }
}

void Player::onCollisionExit(Collider* other) {
    Entity::onCollisionExit(other);

    if (!other) return;

    if (other->getCollisionLayer() & static_cast<int>(CollisionLayer::Platform)) {
        m_isOnWall = false;
        m_isWallSliding = false;
    }

    if (m_isGrounded && other->getCollisionLayer() & static_cast<int>(CollisionLayer::Platform)) {
        m_coyoteTimer = m_coyoteTime;
    }
}

bool Player::takeDamage(const DamageInfo& damageInfo) {
    bool result = Entity::takeDamage(damageInfo);

    if (result) {
        EventSystem::getInstance()->triggerEvent("PlayerDamaged", {
            {"player", this},
            {"damage", damageInfo.amount}
            });
    }

    return result;
}

void Player::onDeath() {
    Entity::onDeath();

    m_lives--;

    EventSystem::getInstance()->triggerEvent("PlayerDied", {
        {"player", this},
        {"livesRemaining", m_lives},
        {"position", m_position}
        });

    if (m_lives <= 0) {
        EventSystem::getInstance()->triggerEvent("GameOver", {
            {"score", m_score}
            });
    }
    else {
        EventSystem::getInstance()->triggerEvent("RespawnPlayer", {
            {"delay", 2.0f}
            });
    }
}

void Player::updateMovement(float dt) {
    if (m_state == EntityState::Dead || m_state == EntityState::Dashing) return;

    float moveX = 0.0f;

    if (isActionActive(PlayerAction::MoveLeft)) {
        moveX -= 1.0f;
    }
    if (isActionActive(PlayerAction::MoveRight)) {
        moveX += 1.0f;
    }

    move(moveX, 0.0f);

    if (isActionActive(PlayerAction::Jump)) {
        if (canJump() || (m_isWallSliding && m_hasWallJump) || (m_jumpsRemaining > 0 && m_hasDoubleJump)) {
            jump();
            setAction(PlayerAction::Jump, false);
        }
        else {
            m_jumpBuffered = true;
            m_jumpBufferTimer = m_jumpBufferTime;
        }
    }

    if (isActionActive(PlayerAction::Attack) && m_canAttack) {
        attack();
        setAction(PlayerAction::Attack, false);
    }

    if (isActionActive(PlayerAction::Dash) && m_canDash && m_hasDash) {
        dash();
        setAction(PlayerAction::Dash, false);
    }

    if (isActionActive(PlayerAction::Interact)) {
        interact();
        setAction(PlayerAction::Interact, false);
    }
}

void Player::updateJump(float dt) {
    if (m_state == EntityState::Jumping && !isActionActive(PlayerAction::Jump)) {
        sf::Vector2f vel = getVelocity();
        if (vel.y < 0) {
            vel.y *= 0.5f;
            setVelocity(vel);
        }
    }

    if (m_state == EntityState::Jumping) {
        sf::Vector2f vel = getVelocity();
        if (vel.y >= 0) {
            setState(EntityState::Falling);
        }
    }
}

void Player::updateDash(float dt) {
    if (m_state == EntityState::Dashing) {
        m_stateTimer -= dt;
        if (m_stateTimer <= 0) {
            if (m_isGrounded) {
                setState(EntityState::Idle);
            }
            else {
                setState(EntityState::Falling);
            }
        }
    }
}

void Player::updateAttack(float dt) {
}

void Player::updateWallSlide(float dt) {
    if (m_isWallSliding) {
        sf::Vector2f vel = getVelocity();
        if (vel.y > m_wallSlideSpeed) {
            setVelocity(vel.x, m_wallSlideSpeed);
        }

        playAnimation("wall_slide");
    }
}

void Player::checkGrounded() {
}

void Player::applyGravity(float dt) {
}

void Player::updateAnimationState() {
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
    case EntityState::Dashing:
        playAnimation("dash");
        break;
    case EntityState::Attacking:
        if (m_currentAnimation != "attack") {
            playAnimation("attack");
        }
        break;
    case EntityState::Hit:
        playAnimation("hit");
        break;
    case EntityState::Dead:
        playAnimation("death");
        break;
    }
}

void Player::loadAnimations() {
    RessourceManager* resourceManager = RessourceManager::getInstance();

    if (resourceManager->loadTexture("player_idle", "Assets/Textures/player_idle.png")) {
        m_texture = std::make_unique<sf::Texture>(*resourceManager->getTexture("player_idle"));
        m_sprite.setTexture(*m_texture);
        m_sprite.setOrigin(m_texture->getSize().x / 2.0f, m_texture->getSize().y / 2.0f);
    }
}
