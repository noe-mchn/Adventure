#pragma once

#include "Entity.h"
#include <functional>

enum class ObjectType {
    Platform,
    MovingPlatform,
    Pickup,
    Trigger,
    Hazard,
    Checkpoint,
    Door,
    Switch,
    Decoration
};

class GameplayObject : public Entity {
protected:
    ObjectType m_objectType;
    bool m_isInteractable;

    std::function<void(Entity*)> m_onInteractCallback;

public:
    GameplayObject(ObjectType type = ObjectType::Decoration);

    void update(float dt) override;
    void initialize() override;

    void setObjectType(ObjectType type);
    ObjectType getObjectType() const;

    void setInteractable(bool interactable);
    bool isInteractable() const;

    void setInteractCallback(const std::function<void(Entity*)>& callback);

    virtual bool interact(Entity* activator);

    void onCollisionEnter(Collider* other) override;
};

class Pickup : public GameplayObject {
private:
    std::string m_pickupType;
    int m_value;
    float m_bobHeight;
    float m_bobSpeed;
    float m_bobTime;
    sf::Vector2f m_initialPosition;

public:
    Pickup(const std::string& type = "coin", int value = 1);

    void update(float dt) override;
    void initialize() override;

    void setPickupType(const std::string& type);
    const std::string& getPickupType() const;

    void setValue(int value);
    int getValue() const;

    bool interact(Entity* activator) override;

    void setBobHeight(float height);
    void setBobSpeed(float speed);
};

class Platform : public GameplayObject {
private:
    bool m_isMoving;
    bool m_isFalling;
    bool m_isOneWay;

    std::vector<sf::Vector2f> m_waypoints;
    int m_currentWaypoint;
    float m_moveSpeed;
    bool m_isLooping;

    float m_fallDelay;
    float m_fallTimer;
    bool m_triggerFall;

public:
    Platform(bool isMoving = false, bool isFalling = false);

    void update(float dt) override;
    void initialize() override;

    void setMoving(bool moving);
    bool isMoving() const;

    void setFalling(bool falling);
    bool isFalling() const;

    void setOneWay(bool oneWay);
    bool isOneWay() const;

    void addWaypoint(const sf::Vector2f& waypoint);
    void clearWaypoints();
    void setMoveSpeed(float speed);
    float getMoveSpeed() const;
    void setLooping(bool looping);
    bool isLooping() const;

    void setFallDelay(float delay);
    float getFallDelay() const;

    void onCollisionEnter(Collider* other) override;
};

class Hazard : public GameplayObject {
private:
    int m_damage;
    float m_knockbackForce;
    bool m_isActive;
    float m_activationDelay;
    float m_activationTimer;
    bool m_isToggling;

public:
    Hazard(int damage = 10);

    void update(float dt) override;
    void initialize() override;

    void setDamage(int damage);
    int getDamage() const;

    void setKnockbackForce(float force);
    float getKnockbackForce() const;

    void setActive(bool active, float delay = 0.0f);
    bool isActive() const;

    void setToggling(bool toggling, float interval = 2.0f);
    bool isToggling() const;

    void onCollisionEnter(Collider* other) override;
};

class Trigger : public GameplayObject {
private:
    bool m_isTriggered;
    bool m_triggerOnce;
    std::string m_triggerTag;

    std::function<void(Entity*)> m_triggerCallback;

public:
    Trigger(bool triggerOnce = true);

    void update(float dt) override;
    void initialize() override;

    void setTriggerOnce(bool once);
    bool isTriggerOnce() const;

    void setTriggered(bool triggered);
    bool isTriggered() const;

    void setTriggerTag(const std::string& tag);
    const std::string& getTriggerTag() const;

    void setTriggerCallback(const std::function<void(Entity*)>& callback);

    void onCollisionEnter(Collider* other) override;
};