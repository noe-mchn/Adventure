#include "Entity.h"
#include "PhysicsEngine.h"
#include "DamageSystem.h"
#include "EventSystem.h"
#include "RessourceManager.h"
#include <cmath>
#include <iostream>

class Animation {
public:
    Animation() = default;
    virtual ~Animation() = default;

    virtual void update(float dt) {}
    virtual bool isFinished() const { return false; }
    virtual void reset() {}
    virtual void apply(sf::Sprite& sprite) {}
    virtual void setSpeed(float speed) {}
    float getSpeed() const { return 1.0f; }
};

Entity::Entity(EntityType type)
    : m_type(type),
    m_name("Entity"),
    m_active(true),
    m_visible(true),
    m_position(0.0f, 0.0f),
    m_scale(1.0f, 1.0f),
    m_rotation(0.0f),
    m_velocity(0.0f, 0.0f),
    m_acceleration(0.0f, 0.0f),
    m_size(32.0f, 32.0f),
    m_color(sf::Color::White),
    m_opacity(1.0f),
    m_state(EntityState::Idle),
    m_stateTimer(0.0f),
    m_facingRight(true),
    m_speed(200.0f),
    m_jumpForce(500.0f),
    m_canJump(true),
    m_isGrounded(false),
    m_health(100),
    m_maxHealth(100),
    m_invulnerable(false),
    m_invulnerabilityTimer(0.0f),
    m_level(nullptr)
{
    m_currentAnimation = "";
}

Entity::~Entity() {
}

void Entity::update(float dt) {
    updatePhysics(dt);
    updateAnimation(dt);
    updateInvulnerability(dt);

    if (m_stateTimer > 0.0f) {
        m_stateTimer -= dt;
        if (m_stateTimer <= 0.0f) {
            m_stateTimer = 0.0f;
            if (m_state == EntityState::Hit || m_state == EntityState::Attacking) {
                setState(EntityState::Idle);
            }
        }
    }
}

void Entity::render(sf::RenderWindow& window) {
    if (!m_visible) return;

    if (m_texture) {
        window.draw(m_sprite);
    }
    else {
        sf::RectangleShape shape(m_size);
        shape.setPosition(m_position);
        shape.setFillColor(m_color);
        shape.setRotation(m_rotation);
        shape.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
        window.draw(shape);
    }
}

void Entity::handleEvents(const sf::Event& event) {
}

void Entity::initialize() {
    if (!m_physicsBody) {
        m_physicsBody = std::make_unique<PhysicsBody>(this);
        PhysicsEngine::getInstance()->registerBody(m_physicsBody.get());
    }

    if (!m_collider) {
        m_collider = std::make_unique<BoxCollider>(this, m_size);
        CollisionManager::getInstance()->registerCollider(m_collider.get());

        m_collider->setCollisionCallback([this](Collider* other, bool isEnter) {
            if (isEnter) {
                onCollisionEnter(other);
            }
            else {
                onCollisionExit(other);
            }
            });
    }

    onSpawn();
}

void Entity::setPosition(const sf::Vector2f& position) {
    m_position = position;
    if (m_sprite.getTexture()) {
        m_sprite.setPosition(m_position);
    }
}

void Entity::setPosition(float x, float y) {
    setPosition(sf::Vector2f(x, y));
}

sf::Vector2f Entity::getPosition() const {
    return m_position;
}

void Entity::setVelocity(const sf::Vector2f& velocity) {
    m_velocity = velocity;
    if (m_physicsBody) {
        m_physicsBody->setVelocity(velocity);
    }
}

void Entity::setVelocity(float x, float y) {
    setVelocity(sf::Vector2f(x, y));
}

sf::Vector2f Entity::getVelocity() const {
    if (m_physicsBody) {
        return m_physicsBody->getVelocity();
    }
    return m_velocity;
}

void Entity::setAcceleration(const sf::Vector2f& acceleration) {
    m_acceleration = acceleration;
    if (m_physicsBody) {
        m_physicsBody->setAcceleration(acceleration);
    }
}

void Entity::setAcceleration(float x, float y) {
    setAcceleration(sf::Vector2f(x, y));
}

sf::Vector2f Entity::getAcceleration() const {
    if (m_physicsBody) {
        return m_physicsBody->getAcceleration();
    }
    return m_acceleration;
}

void Entity::setScale(const sf::Vector2f& scale) {
    m_scale = scale;
    if (m_sprite.getTexture()) {
        m_sprite.setScale(m_scale);
    }
}

void Entity::setScale(float x, float y) {
    setScale(sf::Vector2f(x, y));
}

sf::Vector2f Entity::getScale() const {
    return m_scale;
}

void Entity::setRotation(float rotation) {
    m_rotation = rotation;
    if (m_sprite.getTexture()) {
        m_sprite.setRotation(m_rotation);
    }
}

float Entity::getRotation() const {
    return m_rotation;
}

void Entity::setSize(const sf::Vector2f& size) {
    m_size = size;

    if (m_collider && m_collider->getType() == ColliderType::Box) {
        static_cast<BoxCollider*>(m_collider.get())->setSize(m_size);
    }
}

void Entity::setSize(float width, float height) {
    setSize(sf::Vector2f(width, height));
}

sf::Vector2f Entity::getSize() const {
    return m_size;
}

void Entity::setColor(const sf::Color& color) {
    m_color = color;
    if (m_sprite.getTexture()) {
        m_sprite.setColor(m_color);
    }
}

sf::Color Entity::getColor() const {
    return m_color;
}

void Entity::setOpacity(float opacity) {
    m_opacity = std::max(0.0f, std::min(1.0f, opacity));
    sf::Color color = getColor();
    color.a = static_cast<sf::Uint8>(255 * m_opacity);
    setColor(color);
}

float Entity::getOpacity() const {
    return m_opacity;
}

void Entity::setState(EntityState state) {
    if (m_state == state) return;

    EntityState oldState = m_state;
    m_state = state;

    switch (state) {
    case EntityState::Attacking:
        m_stateTimer = 0.3f;
        break;
    case EntityState::Hit:
        m_stateTimer = 0.5f;
        break;
    case EntityState::Dashing:
        m_stateTimer = 0.2f;
        break;
    default:
        m_stateTimer = 0.0f;
        break;
    }

    switch (state) {
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
    case EntityState::Dashing:
        playAnimation("dash");
        break;
    case EntityState::Hit:
        playAnimation("hit");
        break;
    case EntityState::Dead:
        playAnimation("death");
        break;
    }

    EventSystem::getInstance()->triggerEvent("EntityStateChanged", {
        {"entity", this},
        {"oldState", static_cast<int>(oldState)},
        {"newState", static_cast<int>(state)}
        });
}

EntityState Entity::getState() const {
    return m_state;
}

void Entity::setFacingRight(bool facingRight) {
    if (m_facingRight != facingRight) {
        m_facingRight = facingRight;

        if (m_sprite.getTexture()) {
            if (m_facingRight) {
                m_sprite.setScale(std::abs(m_scale.x), m_scale.y);
            }
            else {
                m_sprite.setScale(-std::abs(m_scale.x), m_scale.y);
            }
        }
    }
}

bool Entity::isFacingRight() const {
    return m_facingRight;
}

sf::Vector2f Entity::getFacingDirection() const {
    return sf::Vector2f(m_facingRight ? 1.0f : -1.0f, 0.0f);
}

void Entity::setSpeed(float speed) {
    m_speed = speed;
}

float Entity::getSpeed() const {
    return m_speed;
}

void Entity::setJumpForce(float force) {
    m_jumpForce = force;
}

float Entity::getJumpForce() const {
    return m_jumpForce;
}

bool Entity::canJump() const {
    return m_canJump && m_isGrounded;
}

bool Entity::isGrounded() const {
    return m_isGrounded;
}

void Entity::addAnimation(const std::string& name, std::unique_ptr<Animation> animation) {
    m_animations[name] = std::move(animation);
}

void Entity::playAnimation(const std::string& name, float speed) {
    auto it = m_animations.find(name);
    if (it != m_animations.end()) {
        m_currentAnimation = name;
        it->second->reset();
        it->second->setSpeed(speed);
    }
    else {
        std::cout << "Warning: Animation '" << name << "' not found for entity " << m_name << std::endl;
    }
}

Animation* Entity::getCurrentAnimation() const {
    auto it = m_animations.find(m_currentAnimation);
    if (it != m_animations.end()) {
        return it->second.get();
    }
    return nullptr;
}

void Entity::playSound(const std::string& name, float volume) {
    EventSystem::getInstance()->triggerEvent("PlaySound", {
        {"sound", name},
        {"volume", volume},
        {"position", m_position}
        });
}

void Entity::setPhysicsBody(std::unique_ptr<PhysicsBody> body) {
    if (m_physicsBody) {
        PhysicsEngine::getInstance()->unregisterBody(m_physicsBody.get());
    }

    m_physicsBody = std::move(body);

    if (m_physicsBody) {
        PhysicsEngine::getInstance()->registerBody(m_physicsBody.get());
    }
}

PhysicsBody* Entity::getPhysicsBody() const {
    return m_physicsBody.get();
}

void Entity::setCollider(std::unique_ptr<Collider> collider) {
    if (m_collider) {
        CollisionManager::getInstance()->unregisterCollider(m_collider.get());
    }

    m_collider = std::move(collider);

    if (m_collider) {
        CollisionManager::getInstance()->registerCollider(m_collider.get());

        m_collider->setCollisionCallback([this](Collider* other, bool isEnter) {
            if (isEnter) {
                onCollisionEnter(other);
            }
            else {
                onCollisionExit(other);
            }
            });
    }
}

Collider* Entity::getCollider() const {
    return m_collider.get();
}

void Entity::onCollisionEnter(Collider* other) {
    if (!other || !other->getOwner()) return;

    if (other->getCollisionLayer() & static_cast<int>(CollisionLayer::Platform)) {
        sf::FloatRect thisBounds = getBounds();
        sf::FloatRect otherBounds = other->getBounds();

        float thisBottom = thisBounds.top + thisBounds.height;
        float thisHorizontalCenter = thisBounds.left + thisBounds.width / 2.0f;

        if (std::abs(thisBottom - otherBounds.top) < 10.0f &&
            thisHorizontalCenter >= otherBounds.left &&
            thisHorizontalCenter <= otherBounds.left + otherBounds.width) {

            m_isGrounded = true;
            m_canJump = true;

            if (m_state == EntityState::Falling) {
                setState(EntityState::Idle);
            }

            if (m_physicsBody) {
                m_physicsBody->setGrounded(true);
            }
        }
    }
}

void Entity::onCollisionExit(Collider* other) {
    if (!other || !other->getOwner()) return;

    if (other->getCollisionLayer() & static_cast<int>(CollisionLayer::Platform)) {
        m_isGrounded = false;

        if (m_physicsBody) {
            m_physicsBody->setGrounded(false);
        }

        if (m_state == EntityState::Idle || m_state == EntityState::Walking) {
            setState(EntityState::Falling);
        }
    }
}

bool Entity::takeDamage(const DamageInfo& damageInfo) {
    if (m_invulnerable || m_state == EntityState::Dead) return false;

    m_health -= static_cast<int>(damageInfo.amount);

    if (m_health <= 0) {
        m_health = 0;
        kill();
        return true;
    }

    setState(EntityState::Hit);

    if (m_physicsBody && damageInfo.knockbackForce > 0) {
        sf::Vector2f knockback = damageInfo.knockbackDirection * damageInfo.knockbackForce;
        m_physicsBody->applyImpulse(knockback);
    }

    setInvulnerable(true, 1.0f);

    playSound("hit", 1.0f);

    return true;
}

void Entity::heal(int amount) {
    if (m_state == EntityState::Dead) return;

    m_health += amount;
    if (m_health > m_maxHealth) {
        m_health = m_maxHealth;
    }

    EventSystem::getInstance()->triggerEvent("EntityHealed", {
        {"entity", this},
        {"amount", amount}
        });
}

void Entity::kill() {
    if (m_state == EntityState::Dead) return;

    m_health = 0;
    setState(EntityState::Dead);

    onDeath();
}

void Entity::setHealth(int health) {
    m_health = health;
    if (m_health > m_maxHealth) {
        m_health = m_maxHealth;
    }
}

int Entity::getHealth() const {
    return m_health;
}

void Entity::setMaxHealth(int maxHealth) {
    m_maxHealth = maxHealth;
    if (m_health > m_maxHealth) {
        m_health = m_maxHealth;
    }
}

int Entity::getMaxHealth() const {
    return m_maxHealth;
}

void Entity::setInvulnerable(bool invulnerable, float duration) {
    m_invulnerable = invulnerable;
    m_invulnerabilityTimer = duration;

    if (m_invulnerable) {
        setOpacity(0.7f);
    }
    else {
        setOpacity(1.0f);
    }

    if (m_invulnerable && duration > 0) {
        DamageSystem::getInstance()->setInvincible(this, duration);
    }
}

bool Entity::isInvulnerable() const {
    return m_invulnerable;
}

void Entity::setType(EntityType type) {
    m_type = type;
}

EntityType Entity::getType() const {
    return m_type;
}

void Entity::setName(const std::string& name) {
    m_name = name;
}

const std::string& Entity::getName() const {
    return m_name;
}

void Entity::setActive(bool active) {
    if (m_active != active) {
        m_active = active;

        if (m_active) {
            onActivate();
        }
        else {
            onDeactivate();
        }
    }
}

bool Entity::isActive() const {
    return m_active;
}

void Entity::setVisible(bool visible) {
    m_visible = visible;
}

bool Entity::isVisible() const {
    return m_visible;
}

void Entity::setLevel(Level* level) {
    m_level = level;
}

Level* Entity::getLevel() const {
    return m_level;
}

sf::FloatRect Entity::getBounds() const {
    if (m_collider) {
        return m_collider->getBounds();
    }

    return sf::FloatRect(
        m_position.x - m_size.x / 2.0f,
        m_position.y - m_size.y / 2.0f,
        m_size.x,
        m_size.y
    );
}

bool Entity::intersects(const Entity& other) const {
    return getBounds().intersects(other.getBounds());
}

bool Entity::contains(const sf::Vector2f& point) const {
    return getBounds().contains(point);
}

float Entity::distanceTo(const Entity& other) const {
    sf::Vector2f diff = m_position - other.getPosition();
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

float Entity::distanceTo(const sf::Vector2f& point) const {
    sf::Vector2f diff = m_position - point;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

void Entity::onActivate() {
}

void Entity::onDeactivate() {
}

void Entity::onSpawn() {
}

void Entity::onDeath() {
    EventSystem::getInstance()->triggerEvent("EntityDied", {
        {"entity", this},
        {"type", static_cast<int>(m_type)},
        {"position", m_position}
        });

    if (m_physicsBody) {
        PhysicsEngine::getInstance()->unregisterBody(m_physicsBody.get());
    }

    if (m_collider) {
        CollisionManager::getInstance()->unregisterCollider(m_collider.get());
    }
}

void Entity::updatePhysics(float dt) {
    if (m_physicsBody) {
        m_position = getPosition();
    }

    if (m_sprite.getTexture()) {
        m_sprite.setPosition(m_position);
    }

    if (m_isGrounded && m_state != EntityState::Jumping && m_physicsBody) {
        sf::Vector2f vel = m_physicsBody->getVelocity();
        if (vel.y > 50.0f) {
            m_isGrounded = false;
            setState(EntityState::Falling);
        }
    }
}

void Entity::updateAnimation(float dt) {
    auto anim = getCurrentAnimation();
    if (anim) {
        anim->update(dt);
        anim->apply(m_sprite);
    }

    if (anim && anim->isFinished()) {
        if (m_state == EntityState::Attacking) {
            setState(EntityState::Idle);
        }
        else if (m_state == EntityState::Dead) {
            setActive(false);
        }
    }
}

void Entity::updateInvulnerability(float dt) {
    if (m_invulnerable && m_invulnerabilityTimer > 0) {
        m_invulnerabilityTimer -= dt;

        if (static_cast<int>(m_invulnerabilityTimer * 10.0f) % 2 == 0) {
            setOpacity(0.7f);
        }
        else {
            setOpacity(1.0f);
        }

        if (m_invulnerabilityTimer <= 0) {
            m_invulnerable = false;
            m_invulnerabilityTimer = 0;
            setOpacity(1.0f);
        }
    }
}
