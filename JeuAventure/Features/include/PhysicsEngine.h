#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "CollisionManager.h"

class Entity;

struct PhysicsProperties {
    float mass;
    float gravityScale;
    bool affectedByGravity;
    float friction;
    float restitution;
    bool isKinematic;
    bool isTrigger;

    PhysicsProperties()
        : mass(1.0f),
        gravityScale(1.0f),
        affectedByGravity(true),
        friction(0.2f),
        restitution(0.0f),
        isKinematic(false),
        isTrigger(false)
    {
    }
};

class PhysicsBody {
private:
    Entity* m_owner;
    PhysicsProperties m_properties;
    bool m_isOnWall;
    bool m_isOnCeiling;
    sf::Vector2f m_groundNormal;
    sf::Vector2f m_wallNormal;

public:
    PhysicsBody(Entity* owner);

    Entity* getOwner() const;
    void setVelocity(const sf::Vector2f& velocity);
    const sf::Vector2f& getVelocity() const;
    void setAcceleration(const sf::Vector2f& acceleration);
    const sf::Vector2f& getAcceleration() const;

    void setProperties(const PhysicsProperties& properties);
    const PhysicsProperties& getProperties() const;

    void PhysicsBody::resetVerticalVelocity();

    bool isGrounded() const;
    bool isOnWall() const;
    bool isOnCeiling() const;
    const sf::Vector2f& getGroundNormal() const;
    const sf::Vector2f& getWallNormal() const;

    void applyForce(const sf::Vector2f& force);
    void applyImpulse(const sf::Vector2f& impulse);
    void resetForces();

    void update(float dt);

    void setGrounded(bool grounded, const sf::Vector2f& normal = sf::Vector2f(0, -1));
    void setOnWall(bool onWall, const sf::Vector2f& normal = sf::Vector2f(-1, 0));
    void setOnCeiling(bool onCeiling);

    sf::Vector2f m_velocity;
    sf::Vector2f m_acceleration;
    sf::Vector2f m_force;
    bool m_isGrounded;
};

class PhysicsEngine {
private:
    static PhysicsEngine* s_instance;

    std::vector<PhysicsBody*> m_bodies;
    sf::Vector2f m_gravity;
    int m_velocityIterations;
    int m_positionIterations;
    CollisionManager* m_collisionManager;

    void integrateForces(PhysicsBody* body, float dt);
    void integrateVelocities(PhysicsBody* body, float dt);
    void resolveCollisions();

    PhysicsEngine();

public:
    PhysicsEngine(const PhysicsEngine&) = delete;
    PhysicsEngine& operator=(const PhysicsEngine&) = delete;

    static PhysicsEngine* getInstance();
    static void cleanup();

    void registerBody(PhysicsBody* body);
    void unregisterBody(PhysicsBody* body);

    void setGravity(const sf::Vector2f& gravity);
    sf::Vector2f getGravity() const;

    void setIterations(int velocityIterations, int positionIterations);

    void update(float dt);
};

