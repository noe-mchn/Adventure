#include "Checkpoint.h"
#include "Level.h"
#include "RessourceManager.h"
#include "EventSystem.h"
#include "Player.h"
#include <cmath>

Checkpoint::Checkpoint()
    : Entity(EntityType::Trigger),
    m_isActive(false),
    m_isActivated(false),
    m_activationRadius(50.0f),
    m_animationTimer(0.0f),
    m_pulseFrequency(2.0f),
    m_pulseAmplitude(0.2f),
    m_showRadius(false) {
    m_name = "Checkpoint";
    m_size = sf::Vector2f(32.0f, 32.0f);
    m_radiusVisual.setRadius(m_activationRadius);
    m_radiusVisual.setFillColor(sf::Color(0, 200, 255, 40));
    m_radiusVisual.setOutlineColor(sf::Color(0, 150, 255));
    m_radiusVisual.setOutlineThickness(2.0f);
    m_radiusVisual.setOrigin(m_activationRadius, m_activationRadius);
}

Checkpoint::Checkpoint(const sf::Vector2f& position, float radius)
    : Entity(EntityType::Trigger),
    m_isActive(false),
    m_isActivated(false),
    m_activationRadius(radius),
    m_animationTimer(0.0f),
    m_pulseFrequency(2.0f),
    m_pulseAmplitude(0.2f),
    m_showRadius(false) {
    m_name = "Checkpoint";
    m_size = sf::Vector2f(32.0f, 32.0f);
    setPosition(position);
    m_radiusVisual.setRadius(m_activationRadius);
    m_radiusVisual.setFillColor(sf::Color(0, 200, 255, 40));
    m_radiusVisual.setOutlineColor(sf::Color(0, 150, 255));
    m_radiusVisual.setOutlineThickness(2.0f);
    m_radiusVisual.setOrigin(m_activationRadius, m_activationRadius);
}

void Checkpoint::initialize() {
    Entity::initialize();
    RessourceManager* resourceManager = RessourceManager::getInstance();
    if (resourceManager->loadTexture("checkpoint", "Assets/Textures/checkpoint.png")) {
        m_texture = std::make_unique<sf::Texture>(*resourceManager->getTexture("checkpoint"));
        m_sprite.setTexture(*m_texture);
        sf::Vector2u textureSize = m_texture->getSize();
        m_sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
    }
    if (m_collider) {
        m_collider->setIsTrigger(true);
        m_collider->setCollisionLayer(static_cast<int>(CollisionLayer::Trigger));
        float diameter = m_activationRadius * 2;
        static_cast<BoxCollider*>(m_collider.get())->setSize(sf::Vector2f(diameter, diameter));
    }
}

void Checkpoint::update(float dt) {
    m_animationTimer += dt;
    if (m_isActive) {
        float scale = 1.0f + std::sin(m_animationTimer * m_pulseFrequency * 2) * m_pulseAmplitude;
        m_sprite.setScale(scale, scale);
        m_sprite.setColor(sf::Color(255, 255, 255));
    }
    else if (m_isActivated) {
        float scale = 1.0f + std::sin(m_animationTimer * m_pulseFrequency) * (m_pulseAmplitude / 2);
        m_sprite.setScale(scale, scale);
        m_sprite.setColor(sf::Color(180, 180, 255));
    }
    else {
        float scale = 1.0f + std::sin(m_animationTimer * m_pulseFrequency / 2) * (m_pulseAmplitude / 3);
        m_sprite.setScale(scale, scale);
        m_sprite.setColor(sf::Color(150, 150, 150));
    }
    if (m_showRadius) {
        float radiusScale = 1.0f + std::sin(m_animationTimer * 1.5f) * 0.1f;
        m_radiusVisual.setRadius(m_activationRadius * radiusScale);
        m_radiusVisual.setOrigin(m_activationRadius * radiusScale, m_activationRadius * radiusScale);
        m_radiusVisual.setPosition(m_position);
    }
    if (m_level && !m_isActive) {
        Player* player = m_level->getPlayer();
        if (player) {
            float distance = distanceTo(*player);
            if (distance <= m_activationRadius) {
                activate();
            }
        }
    }
    Entity::update(dt);
}

void Checkpoint::render(sf::RenderWindow& window) {
    if (m_showRadius) {
        window.draw(m_radiusVisual);
    }
    Entity::render(window);
}

void Checkpoint::onCollisionEnter(Collider* other) {
    if (!other || !other->getOwner()) return;
    if (other->getOwner()->getType() == EntityType::Player) {
        activate();
    }
}

void Checkpoint::activate() {
    if (m_isActive) return;
    m_isActive = true;
    m_isActivated = true;
    if (m_level) {
        m_level->activateCheckpoint(this);
    }
    if (m_onActivateCallback) {
        m_onActivateCallback();
    }
    EventSystem::getInstance()->triggerEvent("CheckpointActivated", {
        {"checkpoint", this},
        {"position", m_position}
        });
    playSound("checkpoint_activated", 1.0f);
}

void Checkpoint::deactivate() {
    if (!m_isActive) return;
    m_isActive = false;
}

bool Checkpoint::isActive() const {
    return m_isActive;
}

bool Checkpoint::isActivated() const {
    return m_isActivated;
}

void Checkpoint::setActivationRadius(float radius) {
    m_activationRadius = radius;
    m_radiusVisual.setRadius(m_activationRadius);
    m_radiusVisual.setOrigin(m_activationRadius, m_activationRadius);
    if (m_collider) {
        float diameter = m_activationRadius * 2;
        static_cast<BoxCollider*>(m_collider.get())->setSize(sf::Vector2f(diameter, diameter));
    }
}

float Checkpoint::getActivationRadius() const {
    return m_activationRadius;
}

void Checkpoint::setShowRadius(bool show) {
    m_showRadius = show;
}

void Checkpoint::setOnActivateCallback(const std::function<void()>& callback) {
    m_onActivateCallback = callback;
}

