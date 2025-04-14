#include "PhysicsEngine.h"
#include <algorithm>
#include <iostream>

class Entity {
public:
    virtual sf::Vector2f getPosition() const { return sf::Vector2f(0, 0); }
    virtual void setPosition(const sf::Vector2f& position) {}
    virtual Collider* getCollider() const { return nullptr; }
};

PhysicsBody::PhysicsBody(Entity* owner)
    : m_owner(owner),
    m_velocity(0, 0),
    m_acceleration(0, 0),
    m_force(0, 0),
    m_isGrounded(false),
    m_isOnWall(false),
    m_isOnCeiling(false),
    m_groundNormal(0, -1),
    m_wallNormal(-1, 0)
{
}

Entity* PhysicsBody::getOwner() const {
    return m_owner;
}

void PhysicsBody::setVelocity(const sf::Vector2f& velocity) {
    m_velocity = velocity;
}

const sf::Vector2f& PhysicsBody::getVelocity() const {
    return m_velocity;
}

void PhysicsBody::setAcceleration(const sf::Vector2f& acceleration) {
    m_acceleration = acceleration;
}

const sf::Vector2f& PhysicsBody::getAcceleration() const {
    return m_acceleration;
}

void PhysicsBody::setProperties(const PhysicsProperties& properties) {
    m_properties = properties;

    if (m_owner) {
        Collider* collider = m_owner->getCollider();
        if (collider) {
            collider->setIsTrigger(m_properties.isTrigger);
        }
    }
}

const PhysicsProperties& PhysicsBody::getProperties() const {
    return m_properties;
}

bool PhysicsBody::isGrounded() const {
    return m_isGrounded;
}

bool PhysicsBody::isOnWall() const {
    return m_isOnWall;
}

bool PhysicsBody::isOnCeiling() const {
    return m_isOnCeiling;
}

const sf::Vector2f& PhysicsBody::getGroundNormal() const {
    return m_groundNormal;
}

const sf::Vector2f& PhysicsBody::getWallNormal() const {
    return m_wallNormal;
}

void PhysicsBody::applyForce(const sf::Vector2f& force) {
    m_force += force;
}

void PhysicsBody::applyImpulse(const sf::Vector2f& impulse) {
    if (m_properties.mass > 0 && !m_properties.isKinematic) {
        m_velocity += impulse / m_properties.mass;
    }
}

void PhysicsBody::resetForces() {
    m_force = sf::Vector2f(0, 0);
}

void PhysicsBody::update(float dt) {
}

void PhysicsBody::setGrounded(bool grounded, const sf::Vector2f& normal) {
    m_isGrounded = grounded;
    if (grounded) {
        m_groundNormal = normal;
    }
}

void PhysicsBody::setOnWall(bool onWall, const sf::Vector2f& normal) {
    m_isOnWall = onWall;
    if (onWall) {
        m_wallNormal = normal;
    }
}

void PhysicsBody::setOnCeiling(bool onCeiling) {
    m_isOnCeiling = onCeiling;
}

PhysicsEngine* PhysicsEngine::s_instance = nullptr;

PhysicsEngine::PhysicsEngine()
    : m_gravity(0, 980.0f),
    m_velocityIterations(8),
    m_positionIterations(3)
{
    m_collisionManager = CollisionManager::getInstance();
}

PhysicsEngine* PhysicsEngine::getInstance() {
    if (s_instance == nullptr) {
        s_instance = new PhysicsEngine();
    }
    return s_instance;
}

void PhysicsEngine::cleanup() {
    if (s_instance != nullptr) {
        delete s_instance;
        s_instance = nullptr;
    }
}

void PhysicsEngine::registerBody(PhysicsBody* body) {
    if (!body) return;

    auto it = std::find(m_bodies.begin(), m_bodies.end(), body);
    if (it == m_bodies.end()) {
        m_bodies.push_back(body);
    }
}

void PhysicsEngine::unregisterBody(PhysicsBody* body) {
    if (!body) return;

    auto it = std::find(m_bodies.begin(), m_bodies.end(), body);
    if (it != m_bodies.end()) {
        m_bodies.erase(it);
    }
}

void PhysicsEngine::setGravity(const sf::Vector2f& gravity) {
    m_gravity = gravity;
}

sf::Vector2f PhysicsEngine::getGravity() const {
    return m_gravity;
}

void PhysicsEngine::setIterations(int velocityIterations, int positionIterations) {
    m_velocityIterations = velocityIterations;
    m_positionIterations = positionIterations;
}

void PhysicsEngine::update(float dt) {
    for (auto* body : m_bodies) {
        integrateForces(body, dt);
    }

    for (int i = 0; i < m_velocityIterations; ++i) {
        m_collisionManager->checkCollisions();
        resolveCollisions();
    }

    for (auto* body : m_bodies) {
        integrateVelocities(body, dt);
    }

    for (int i = 0; i < m_positionIterations; ++i) {
        m_collisionManager->checkCollisions();
        resolveCollisions();
    }

    for (auto* body : m_bodies) {
        body->resetForces();
    }
}

void PhysicsBody::resetVerticalVelocity() {
    m_velocity.y = 0;
}

void PhysicsEngine::integrateForces(PhysicsBody* body, float dt) {
    if (!body || !body->getOwner()) return;

    const PhysicsProperties& props = body->getProperties();
    if (props.isKinematic) return;

    if (props.affectedByGravity) {
        body->applyForce(m_gravity * props.mass * props.gravityScale);
    }

    sf::Vector2f acceleration = body->m_force / props.mass;
    body->m_velocity += acceleration * dt;

    if (body->m_isGrounded) {
        float frictionAccel = props.friction * 9.8f;
        float frictionDelta = frictionAccel * dt;

        if (std::abs(body->m_velocity.x) <= frictionDelta) {
            body->m_velocity.x = 0;
        }
        else {
            body->m_velocity.x -= frictionDelta * ((body->m_velocity.x > 0) ? 1 : -1);
        }
    }

    const float velocityThreshold = 0.1f;
    if (std::abs(body->m_velocity.x) < velocityThreshold) {
        body->m_velocity.x = 0;
    }

    if (body->m_isGrounded && std::abs(body->m_velocity.y) < velocityThreshold) {
        body->m_velocity.y = 0;
    }
}


void PhysicsEngine::integrateVelocities(PhysicsBody* body, float dt) {
    if (!body || !body->getOwner()) return;

    if (body->getProperties().isKinematic &&
        body->m_velocity.x == 0 &&
        body->m_velocity.y == 0) {
        return;
    }

    sf::Vector2f newPos = body->getOwner()->getPosition() + body->m_velocity * dt;
    body->getOwner()->setPosition(newPos);
}

void PhysicsEngine::resolveCollisions() {
}
