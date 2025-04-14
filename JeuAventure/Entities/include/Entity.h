#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <map>
#include "Collider.h"
#include "PhysicsEngine.h"
#include "DamageSystem.h"

class Animation;

enum class EntityType {
    None,
    Player,
    Enemy,
    Projectile,
    Platform,
    Pickup,
    Trigger,
    Decoration
};

enum class EntityState {
    Idle,
    Walking,
    Running,
    Jumping,
    Falling,
    Attacking,
    Dashing,
    Hit,
    Dead
};

class Entity {
protected:
    EntityType m_type;
    std::string m_name;
    bool m_active;
    bool m_visible;
    sf::Vector2f m_position;
    sf::Vector2f m_scale;
    float m_rotation;
    sf::Vector2f m_velocity;
    sf::Vector2f m_acceleration;
    sf::Vector2f m_size;
    sf::Color m_color;
    float m_opacity;

    EntityState m_state;
    float m_stateTimer;
    bool m_facingRight;

    float m_speed;
    float m_jumpForce;
    bool m_canJump;
    bool m_isGrounded;

    std::unique_ptr<PhysicsBody> m_physicsBody;
    std::unique_ptr<Collider> m_collider;

    sf::Sprite m_sprite;
    std::unique_ptr<sf::Texture> m_texture;
    std::map<std::string, std::unique_ptr<Animation>> m_animations;
    std::string m_currentAnimation;

    int m_health;
    int m_maxHealth;
    bool m_invulnerable;
    float m_invulnerabilityTimer;

    class Level* m_level;

public:
    Entity(EntityType type = EntityType::None);
    virtual ~Entity();

    virtual void update(float dt);
    virtual void render(sf::RenderWindow& window);
    virtual void handleEvents(const sf::Event& event);
    virtual void initialize();

    void setPosition(const sf::Vector2f& position);
    void setPosition(float x, float y);
    sf::Vector2f getPosition() const;

    void setVelocity(const sf::Vector2f& velocity);
    void setVelocity(float x, float y);
    sf::Vector2f getVelocity() const;

    void setAcceleration(const sf::Vector2f& acceleration);
    void setAcceleration(float x, float y);
    sf::Vector2f getAcceleration() const;

    void setScale(const sf::Vector2f& scale);
    void setScale(float x, float y);
    sf::Vector2f getScale() const;

    void setRotation(float rotation);
    float getRotation() const;

    void setSize(const sf::Vector2f& size);
    void setSize(float width, float height);
    sf::Vector2f getSize() const;

    void setColor(const sf::Color& color);
    sf::Color getColor() const;

    void setOpacity(float opacity);
    float getOpacity() const;

    void setState(EntityState state);
    EntityState getState() const;

    void setFacingRight(bool facingRight);
    bool isFacingRight() const;
    sf::Vector2f getFacingDirection() const;

    void setSpeed(float speed);
    float getSpeed() const;

    void setJumpForce(float force);
    float getJumpForce() const;

    bool canJump() const;
    bool isGrounded() const;

    void addAnimation(const std::string& name, std::unique_ptr<Animation> animation);
    void playAnimation(const std::string& name, float speed = 1.0f);
    Animation* getCurrentAnimation() const;

    virtual void playSound(const std::string& name, float volume = 1.0f);

    void setPhysicsBody(std::unique_ptr<PhysicsBody> body);
    PhysicsBody* getPhysicsBody() const;

    void setCollider(std::unique_ptr<Collider> collider);
    Collider* getCollider() const;

    virtual void onCollisionEnter(Collider* other);
    virtual void onCollisionExit(Collider* other);

    virtual bool takeDamage(const DamageInfo& damageInfo);
    virtual void heal(int amount);
    virtual void kill();

    void setHealth(int health);
    int getHealth() const;

    void setMaxHealth(int maxHealth);
    int getMaxHealth() const;

    void setInvulnerable(bool invulnerable, float duration = 0.0f);
    bool isInvulnerable() const;

    void setType(EntityType type);
    EntityType getType() const;

    void setName(const std::string& name);
    const std::string& getName() const;

    void setActive(bool active);
    bool isActive() const;

    void setVisible(bool visible);
    bool isVisible() const;

    void setLevel(Level* level);
    Level* getLevel() const;

    sf::FloatRect getBounds() const;
    bool intersects(const Entity& other) const;
    bool contains(const sf::Vector2f& point) const;
    float distanceTo(const Entity& other) const;
    float distanceTo(const sf::Vector2f& point) const;

    virtual void onActivate();
    virtual void onDeactivate();
    virtual void onSpawn();
    virtual void onDeath();

protected:
    virtual void updatePhysics(float dt);
    virtual void updateAnimation(float dt);
    virtual void updateInvulnerability(float dt);
};
