#include "Objects.h"
#include "EventSystem.h"
#include "RessourceManager.h"
#include <iostream>
#include <cmath>

GameplayObject::GameplayObject(ObjectType type)
    : Entity(EntityType::None),
    m_objectType(type),
    m_isInteractable(false)
{
    m_name = "Object";

    switch (type) {
    case ObjectType::Platform:
    case ObjectType::MovingPlatform:
        setType(EntityType::Platform);
        m_name = "Platform";
        break;

    case ObjectType::Pickup:
        setType(EntityType::Pickup);
        m_name = "Pickup";
        break;

    case ObjectType::Trigger:
        setType(EntityType::Trigger);
        m_name = "Trigger";
        break;

    case ObjectType::Hazard:
        setType(EntityType::Enemy);
        m_name = "Hazard";
        break;

    case ObjectType::Checkpoint:
    case ObjectType::Door:
    case ObjectType::Switch:
        setType(EntityType::Trigger);
        m_isInteractable = true;
        break;

    case ObjectType::Decoration:
    default:
        setType(EntityType::None);
        break;
    }
}

void GameplayObject::update(float dt) {
    Entity::update(dt);
}

void GameplayObject::initialize() {
    Entity::initialize();

    if (m_collider) {
        switch (m_objectType) {
        case ObjectType::Platform:
        case ObjectType::MovingPlatform:
            m_collider->setCollisionLayer(static_cast<int>(CollisionLayer::Platform));
            break;

        case ObjectType::Pickup:
            m_collider->setIsTrigger(true);
            break;

        case ObjectType::Trigger:
            m_collider->setIsTrigger(true);
            break;

        case ObjectType::Hazard:
            m_collider->setIsTrigger(true);
            break;

        default:
            m_collider->setEnabled(false);
            break;
        }
    }
}

void GameplayObject::setObjectType(ObjectType type) {
    m_objectType = type;
}

ObjectType GameplayObject::getObjectType() const {
    return m_objectType;
}

void GameplayObject::setInteractable(bool interactable) {
    m_isInteractable = interactable;
}

bool GameplayObject::isInteractable() const {
    return m_isInteractable;
}

void GameplayObject::setInteractCallback(const std::function<void(Entity*)>& callback) {
    m_onInteractCallback = callback;
}

bool GameplayObject::interact(Entity* activator) {
    if (!m_isInteractable) return false;

    if (m_onInteractCallback) {
        m_onInteractCallback(activator);
        return true;
    }

    return false;
}

void GameplayObject::onCollisionEnter(Collider* other) {
    Entity::onCollisionEnter(other);
}

Pickup::Pickup(const std::string& type, int value)
    : GameplayObject(ObjectType::Pickup),
    m_pickupType(type),
    m_value(value),
    m_bobHeight(5.0f),
    m_bobSpeed(3.0f),
    m_bobTime(0.0f)
{
    m_name = type + "_pickup";
    m_size = sf::Vector2f(24.0f, 24.0f);

    if (type == "coin") {
        m_color = sf::Color::Yellow;
    }
    else if (type == "health") {
        m_color = sf::Color::Red;
    }
    else if (type == "key") {
        m_color = sf::Color(200, 200, 255);
    }
}

void Pickup::update(float dt) {
    m_bobTime += dt;
    float offset = std::sin(m_bobTime * m_bobSpeed) * m_bobHeight;

    setPosition(m_initialPosition.x, m_initialPosition.y + offset);

    setRotation(getRotation() + 40.0f * dt);

    GameplayObject::update(dt);
}

void Pickup::initialize() {
    GameplayObject::initialize();

    m_initialPosition = getPosition();

    if (m_collider) {
        m_collider->setIsTrigger(true);
    }

    RessourceManager* resourceManager = RessourceManager::getInstance();
    std::string textureKey = "pickup_" + m_pickupType;

    if (resourceManager->loadTexture(textureKey, "Assets/Textures/" + textureKey + ".png")) {
        m_texture = std::make_unique<sf::Texture>(*resourceManager->getTexture(textureKey));
        m_sprite.setTexture(*m_texture);
        m_sprite.setOrigin(m_texture->getSize().x / 2.0f, m_texture->getSize().y / 2.0f);
    }
}

void Pickup::setPickupType(const std::string& type) {
    m_pickupType = type;
}

const std::string& Pickup::getPickupType() const {
    return m_pickupType;
}

void Pickup::setValue(int value) {
    m_value = value;
}

int Pickup::getValue() const {
    return m_value;
}

bool Pickup::interact(Entity* activator) {
    return false;
}

void Pickup::setBobHeight(float height) {
    m_bobHeight = height;
}

void Pickup::setBobSpeed(float speed) {
    m_bobSpeed = speed;
}

Platform::Platform(bool isMoving, bool isFalling)
    : GameplayObject(isMoving ? ObjectType::MovingPlatform : ObjectType::Platform),
    m_isMoving(isMoving),
    m_isFalling(isFalling),
    m_isOneWay(false),
    m_currentWaypoint(0),
    m_moveSpeed(100.0f),
    m_isLooping(true),
    m_fallDelay(0.5f),
    m_fallTimer(0.0f),
    m_triggerFall(false)
{
    m_name = isMoving ? "MovingPlatform" : "Platform";

    if (isFalling) {
        m_name = "FallingPlatform";
    }

    m_size = sf::Vector2f(128.0f, 32.0f);
    m_color = sf::Color(150, 150, 150);
}

void Platform::update(float dt) {
    if (m_isMoving && !m_waypoints.empty()) {
        sf::Vector2f target = m_waypoints[m_currentWaypoint];

        sf::Vector2f direction = target - m_position;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance < 5.0f) {
            if (++m_currentWaypoint >= m_waypoints.size()) {
                if (m_isLooping) {
                    m_currentWaypoint = 0;
                }
                else {
                    m_currentWaypoint = m_waypoints.size() - 1;

                    std::reverse(m_waypoints.begin(), m_waypoints.end());
                    m_currentWaypoint = 0;
                }
            }

            setPosition(target);
        }
        else {
            if (distance > 0) {
                direction /= distance;
            }

            sf::Vector2f velocity = direction * m_moveSpeed;
            setVelocity(velocity);
        }
    }

    if (m_isFalling && m_triggerFall) {
        m_fallTimer -= dt;

        if (m_fallTimer <= 0.0f) {
            if (m_physicsBody) {
                PhysicsProperties props = m_physicsBody->getProperties();
                props.affectedByGravity = true;
                m_physicsBody->setProperties(props);

                setVelocity(0.0f, 50.0f);
            }

            EventSystem::getInstance()->triggerEvent("DeactivateAfterDelay", {
                {"entity", this},
                {"delay", 5.0f}
                });
        }
    }

    GameplayObject::update(dt);
}

void Platform::initialize() {
    GameplayObject::initialize();

    if (m_physicsBody) {
        PhysicsProperties props;
        props.mass = 10.0f;
        props.affectedByGravity = false;
        props.isKinematic = !m_isMoving;
        props.friction = 0.1f;
        m_physicsBody->setProperties(props);
    }

    if (m_collider) {
        m_collider->setCollisionLayer(static_cast<int>(CollisionLayer::Platform));
    }

    if (m_isMoving && m_waypoints.empty()) {
        m_waypoints.push_back(m_position);
    }

    RessourceManager* resourceManager = RessourceManager::getInstance();
    std::string textureKey = m_isMoving ? "platform_moving" : "platform_static";

    if (m_isFalling) {
        textureKey = "platform_falling";
    }

    if (resourceManager->loadTexture(textureKey, "Assets/Textures/" + textureKey + ".png")) {
        m_texture = std::make_unique<sf::Texture>(*resourceManager->getTexture(textureKey));
        m_sprite.setTexture(*m_texture);
        m_sprite.setOrigin(m_texture->getSize().x / 2.0f, m_texture->getSize().y / 2.0f);
    }
}

void Platform::setMoving(bool moving) {
    m_isMoving = moving;

    if (m_isMoving) {
        m_objectType = ObjectType::MovingPlatform;
    }
    else {
        m_objectType = ObjectType::Platform;
    }

    if (m_physicsBody) {
        PhysicsProperties props = m_physicsBody->getProperties();
        props.isKinematic = !m_isMoving;
        m_physicsBody->setProperties(props);
    }
}

bool Platform::isMoving() const {
    return m_isMoving;
}

void Platform::setFalling(bool falling) {
    m_isFalling = falling;
}

bool Platform::isFalling() const {
    return m_isFalling;
}

void Platform::setOneWay(bool oneWay) {
    m_isOneWay = oneWay;
}

bool Platform::isOneWay() const {
    return m_isOneWay;
}

void Platform::addWaypoint(const sf::Vector2f& waypoint) {
    m_waypoints.push_back(waypoint);
}

void Platform::clearWaypoints() {
    m_waypoints.clear();
    m_currentWaypoint = 0;
}

void Platform::setMoveSpeed(float speed) {
    m_moveSpeed = speed;
}

float Platform::getMoveSpeed() const {
    return m_moveSpeed;
}

void Platform::setLooping(bool looping) {
    m_isLooping = looping;
}

bool Platform::isLooping() const {
    return m_isLooping;
}

void Platform::setFallDelay(float delay) {
    m_fallDelay = delay;
}

float Platform::getFallDelay() const {
    return m_fallDelay;
}

void Platform::onCollisionEnter(Collider* other) {
    if (!other || !other->getOwner()) return;

    Entity* otherEntity = static_cast<Entity*>(other->getOwner());

    if (m_isOneWay && otherEntity->getType() == EntityType::Player) {
        sf::FloatRect thisBounds = getBounds();
        sf::FloatRect otherBounds = otherEntity->getBounds();

        float otherBottom = otherBounds.top + otherBounds.height;

        if (otherBottom > thisBounds.top) {
            other->setEnabled(false);
            EventSystem::getInstance()->triggerEvent("EnableColliderAfterDelay", {
                {"collider", other},
                {"delay", 0.1f}
                });
            return;
        }
    }

    if (m_isFalling && !m_triggerFall && otherEntity->getType() == EntityType::Player) {
        m_triggerFall = true;
        m_fallTimer = m_fallDelay;

        playSound("platform_crumble", 1.0f);
    }

    GameplayObject::onCollisionEnter(other);
}

Hazard::Hazard(int damage)
    : GameplayObject(ObjectType::Hazard),
    m_damage(damage),
    m_knockbackForce(200.0f),
    m_isActive(true),
    m_activationDelay(0.0f),
    m_activationTimer(0.0f),
    m_isToggling(false)
{
    m_name = "Hazard";
    m_size = sf::Vector2f(32.0f, 32.0f);
    m_color = sf::Color(255, 50, 50);
}

void Hazard::update(float dt) {
    if (m_activationTimer > 0.0f) {
        m_activationTimer -= dt;

        if (m_activationTimer <= 0.0f) {
            m_isActive = !m_isActive;
            m_activationTimer = 0.0f;

            if (m_isToggling) {
                setActive(!m_isActive, m_activationDelay);
            }
        }
    }

    if (m_isActive) {
        setColor(sf::Color(255, 50, 50));
    }
    else {
        setColor(sf::Color(150, 50, 50, 128));
    }

    GameplayObject::update(dt);
}

void Hazard::initialize() {
    GameplayObject::initialize();

    if (m_collider) {
        m_collider->setIsTrigger(true);
    }

    RessourceManager* resourceManager = RessourceManager::getInstance();
    std::string textureKey = "hazard_spikes";

    if (resourceManager->loadTexture(textureKey, "Assets/Textures/" + textureKey + ".png")) {
        m_texture = std::make_unique<sf::Texture>(*resourceManager->getTexture(textureKey));
        m_sprite.setTexture(*m_texture);
        m_sprite.setOrigin(m_texture->getSize().x / 2.0f, m_texture->getSize().y / 2.0f);
    }
}

void Hazard::setDamage(int damage) {
    m_damage = damage;
}

int Hazard::getDamage() const {
    return m_damage;
}

void Hazard::setKnockbackForce(float force) {
    m_knockbackForce = force;
}

float Hazard::getKnockbackForce() const {
    return m_knockbackForce;
}

void Hazard::setActive(bool active, float delay) {
    if (delay <= 0.0f) {
        m_isActive = active;
    }
    else {
        m_activationTimer = delay;
    }
}

bool Hazard::isActive() const {
    return m_isActive;
}

void Hazard::setToggling(bool toggling, float interval) {
    m_isToggling = toggling;
    m_activationDelay = interval;

    if (m_isToggling && m_activationTimer <= 0.0f) {
        setActive(!m_isActive, m_activationDelay);
    }
}

bool Hazard::isToggling() const {
    return m_isToggling;
}

void Hazard::onCollisionEnter(Collider* other) {
    if (!m_isActive || !other || !other->getOwner()) return;

    Entity* otherEntity = static_cast<Entity*>(other->getOwner());

    if (otherEntity->getType() == EntityType::Player) {
        sf::Vector2f knockbackDir = otherEntity->getPosition() - m_position;
        float length = std::sqrt(knockbackDir.x * knockbackDir.x + knockbackDir.y * knockbackDir.y);

        if (length > 0) {
            knockbackDir /= length;
        }
        else {
            knockbackDir = sf::Vector2f(0.0f, -1.0f);
        }

        DamageInfo damageInfo;
        damageInfo.amount = static_cast<float>(m_damage);
        damageInfo.source = this;
        damageInfo.knockbackForce = m_knockbackForce;
        damageInfo.knockbackDirection = knockbackDir;
        damageInfo.type = "hazard";

        otherEntity->takeDamage(damageInfo);

        playSound("hazard_hit", 1.0f);
    }
}

Trigger::Trigger(bool triggerOnce)
    : GameplayObject(ObjectType::Trigger),
    m_isTriggered(false),
    m_triggerOnce(triggerOnce),
    m_triggerTag("")
{
    m_name = "Trigger";
    m_size = sf::Vector2f(64.0f, 64.0f);
    m_color = sf::Color(0, 255, 255, 100);
}

void Trigger::update(float dt) {
    GameplayObject::update(dt);
}

void Trigger::initialize() {
    GameplayObject::initialize();

    if (m_collider) {
        m_collider->setIsTrigger(true);
    }

    m_visible = false;
}

void Trigger::setTriggerOnce(bool once) {
    m_triggerOnce = once;
}

bool Trigger::isTriggerOnce() const {
    return m_triggerOnce;
}

void Trigger::setTriggered(bool triggered) {
    m_isTriggered = triggered;
}

bool Trigger::isTriggered() const {
    return m_isTriggered;
}

void Trigger::setTriggerTag(const std::string& tag) {
    m_triggerTag = tag;
}

const std::string& Trigger::getTriggerTag() const {
    return m_triggerTag;
}

void Trigger::setTriggerCallback(const std::function<void(Entity*)>& callback) {
    m_triggerCallback = callback;
}

void Trigger::onCollisionEnter(Collider* other) {
    if (m_isTriggered && m_triggerOnce) return;

    if (!other || !other->getOwner()) return;

    Entity* otherEntity = static_cast<Entity*>(other->getOwner());

    if (otherEntity->getType() == EntityType::Player) {
        m_isTriggered = true;

        if (m_triggerCallback) {
            m_triggerCallback(otherEntity);
        }

        EventSystem::getInstance()->triggerEvent("TriggerActivated", {
            {"trigger", this},
            {"activator", otherEntity},
            {"tag", m_triggerTag}
            });

        playSound("trigger_activated", 0.5f);
    }
}
