#pragma once

#include "Entity.h"
#include <unordered_map>
#include <SFML/Graphics.hpp>

enum class PlayerAction {
    MoveLeft,
    MoveRight,
    Jump,
    Attack,
    Dash,
    Special,
    Interact
};

class Player : public Entity {
public:
    Player();
    virtual ~Player() = default;

    void initialize() override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    void handleEvents(const sf::Event& event) override;

    void move(float x, float y);
    void jump();
    void attack();
    void dash();
    void interact();

    void setAction(PlayerAction action, bool active);
    bool isActionActive(PlayerAction action) const;

    void setHasDash(bool hasDash);
    bool hasDash() const;

    void setHasDoubleJump(bool hasDoubleJump);
    bool hasDoubleJump() const;

    void setHasWallJump(bool hasWallJump);
    bool hasWallJump() const;

    void resetJumps();
    int getJumpsRemaining() const;
    int getMaxJumps() const;

    void addCoins(int amount);
    int getCoins() const;

    void addScore(int amount);
    int getScore() const;

    void addLives(int amount);
    int getLives() const;

    void onCollisionEnter(Collider* other) override;
    void onCollisionExit(Collider* other) override;
    bool takeDamage(const DamageInfo& damageInfo) override;
    void onDeath() override;

private:
    void updateMovement(float dt);
    void updateJump(float dt);
    void updateDash(float dt);
    void updateAttack(float dt);
    void updateWallSlide(float dt);
    void updateAnimationState();
    void updatePlayerVisuals();
    void checkGrounded();
    void applyGravity(float dt);
    void loadAnimations();
    void playAnimation(const std::string& name);

    std::unordered_map<PlayerAction, bool> m_actions;
    sf::RectangleShape m_playerRect;

    int m_coins;
    int m_score;
    int m_lives;

    float m_dashCooldown;
    float m_dashTimer;
    bool m_canDash;

    float m_attackCooldown;
    float m_attackTimer;
    bool m_canAttack;

    bool m_hasDash;
    bool m_hasDoubleJump;
    bool m_hasWallJump;

    int m_jumpsRemaining;
    int m_maxJumps;
    bool m_isOnWall;
    bool m_isWallSliding;
    float m_wallSlideSpeed;
    sf::Vector2f m_wallNormal;
    float m_wallJumpForce;

    float m_coyoteTime;
    float m_coyoteTimer;

    float m_jumpBufferTime;
    float m_jumpBufferTimer;
    bool m_jumpBuffered;
    std::string m_currentAnimation;
};