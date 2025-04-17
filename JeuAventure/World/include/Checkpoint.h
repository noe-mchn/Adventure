#pragma once

#include <SFML/Graphics.hpp>
#include "Entity.h"

class Checkpoint : public Entity {
private:
    bool m_isActive;
    bool m_isActivated;
    float m_activationRadius;
    sf::CircleShape m_radiusVisual;

    float m_animationTimer;
    float m_pulseFrequency;
    float m_pulseAmplitude;
    bool m_showRadius;

    std::function<void()> m_onActivateCallback;

public:
    Checkpoint();
    Checkpoint(const sf::Vector2f& position, float radius = 2.0f);

    void initialize() override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    void onCollisionEnter(Collider* other) override;

    void activate();
    void deactivate();

    bool isActive() const;
    bool isActivated() const;
    void setActivationRadius(float radius);
    float getActivationRadius() const;
    void setShowRadius(bool show);

    void setOnActivateCallback(const std::function<void()>& callback);
};
