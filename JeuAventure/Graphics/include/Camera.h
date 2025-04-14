#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

class Entity;

enum class CameraMode {
    Follow,
    Fixed,
    Lerp,
    LookAhead,
    Rails
};

class Camera {
private:
    sf::View m_view;
    Entity* m_target;
    CameraMode m_mode;
    sf::Vector2f m_offset;
    sf::Vector2f m_position;
    sf::Vector2f m_targetPosition;
    sf::Vector2f m_bounds;
    sf::Vector2f m_viewportSize;
    float m_zoomLevel;
    float m_rotationAngle;
    float m_lerpFactor;
    float m_lookAheadDistance;
    float m_shakeIntensity;
    float m_shakeTimer;
    float m_shakeDuration;
    bool m_constrainToLevel;

    std::vector<sf::Vector2f> m_rails;
    int m_currentRailPoint;
    float m_railProgress;

    sf::Vector2f m_lastTargetPos;
    sf::Vector2f m_predictedPos;

    std::function<void(Camera&, float)> m_customUpdateFunc;

public:
    Camera();

    void setViewport(const sf::Vector2f& size);
    void setLevelBounds(const sf::Vector2f& bounds);

    void setTarget(Entity* target);
    void setMode(CameraMode mode);
    void setOffset(const sf::Vector2f& offset);
    void setPosition(const sf::Vector2f& position);
    void setTargetPosition(const sf::Vector2f& position);
    void setZoom(float zoom);
    void setRotation(float angle);
    void setLerpFactor(float factor);
    void setLookAheadDistance(float distance);
    void setConstrainToLevel(bool constrain);

    void addRailPoint(const sf::Vector2f& point);
    void clearRailPoints();
    void setRailProgress(float progress);

    void shake(float intensity, float duration);

    void setCustomUpdateFunction(const std::function<void(Camera&, float)>& func);

    void update(float deltaTime);
    void applyTo(sf::RenderWindow& window);

    sf::Vector2f worldToScreenCoords(const sf::Vector2f& worldCoords, const sf::RenderWindow& window) const;
    sf::Vector2f screenToWorldCoords(const sf::Vector2f& screenCoords, const sf::RenderWindow& window) const;

    const sf::View& getView() const;
    sf::Vector2f getPosition() const;
    float getZoom() const;
    float getRotation() const;
    Entity* getTarget() const;
    CameraMode getMode() const;
    sf::Vector2f getCenter() const;
    sf::Vector2f getSize() const;
    sf::Vector2f getOffset() const;
    float getLerpFactor() const;
    float getLookAheadDistance() const;
    bool isConstrainedToLevel() const;

private:
    void updateFollow(float dt);
    void updateFixed(float dt);
    void updateLerp(float dt);
    void updateLookAhead(float dt);
    void updateRails(float dt);
    void updateShake(float dt);
    void constrainToLevelBounds();
};
