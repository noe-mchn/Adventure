#include "Camera.h"
#include "Entity.h"
#include <cmath>

Camera::Camera() :
    m_target(nullptr),
    m_mode(CameraMode::Fixed),
    m_offset(0.0f, 0.0f),
    m_position(0.0f, 0.0f),
    m_targetPosition(0.0f, 0.0f),
    m_bounds(1000.0f, 1000.0f),
    m_viewportSize(800.0f, 600.0f),
    m_zoomLevel(1.0f),
    m_rotationAngle(0.0f),
    m_lerpFactor(5.0f),
    m_lookAheadDistance(100.0f),
    m_shakeIntensity(0.0f),
    m_shakeTimer(0.0f),
    m_shakeDuration(0.0f),
    m_constrainToLevel(true),
    m_currentRailPoint(0),
    m_railProgress(0.0f),
    m_lastTargetPos(0.0f, 0.0f),
    m_predictedPos(0.0f, 0.0f)
{
    m_view.setSize(m_viewportSize);
    m_view.setCenter(0.0f, 0.0f);
}

void Camera::setViewport(const sf::Vector2f& size) {
    m_viewportSize = size;
    m_view.setSize(m_viewportSize.x / m_zoomLevel, m_viewportSize.y / m_zoomLevel);
}

void Camera::setLevelBounds(const sf::Vector2f& bounds) {
    m_bounds = bounds;
}

void Camera::setTarget(Entity* target) {
    m_target = target;
    if (m_target) {
        m_lastTargetPos = m_target->getPosition();
        m_predictedPos = m_lastTargetPos;
    }
}

void Camera::setMode(CameraMode mode) {
    m_mode = mode;
}

void Camera::setOffset(const sf::Vector2f& offset) {
    m_offset = offset;
}

void Camera::setPosition(const sf::Vector2f& position) {
    m_position = position;
    m_view.setCenter(m_position);
}

void Camera::setTargetPosition(const sf::Vector2f& position) {
    m_targetPosition = position;
}

void Camera::setZoom(float zoom) {
    if (zoom <= 0.0f) return;

    m_zoomLevel = zoom;
    m_view.setSize(m_viewportSize.x / m_zoomLevel, m_viewportSize.y / m_zoomLevel);
}

void Camera::setRotation(float angle) {
    m_rotationAngle = angle;
    m_view.setRotation(m_rotationAngle);
}

void Camera::setLerpFactor(float factor) {
    m_lerpFactor = factor;
}

void Camera::setLookAheadDistance(float distance) {
    m_lookAheadDistance = distance;
}

void Camera::setConstrainToLevel(bool constrain) {
    m_constrainToLevel = constrain;
}

void Camera::addRailPoint(const sf::Vector2f& point) {
    m_rails.push_back(point);
}

void Camera::clearRailPoints() {
    m_rails.clear();
    m_currentRailPoint = 0;
    m_railProgress = 0.0f;
}

void Camera::setRailProgress(float progress) {
    m_railProgress = std::max(0.0f, std::min(1.0f, progress));

    if (m_rails.size() < 2) return;

    float totalLength = 0.0f;
    std::vector<float> segmentLengths;

    for (size_t i = 1; i < m_rails.size(); ++i) {
        sf::Vector2f diff = m_rails[i] - m_rails[i - 1];
        float length = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        segmentLengths.push_back(length);
        totalLength += length;
    }

    float targetDist = m_railProgress * totalLength;
    float currentDist = 0.0f;

    for (size_t i = 0; i < segmentLengths.size(); ++i) {
        if (currentDist + segmentLengths[i] >= targetDist) {
            float segmentProgress = (targetDist - currentDist) / segmentLengths[i];
            m_position = m_rails[i] + segmentProgress * (m_rails[i + 1] - m_rails[i]);
            break;
        }
        currentDist += segmentLengths[i];
    }
}

void Camera::shake(float intensity, float duration) {
    m_shakeIntensity = intensity;
    m_shakeTimer = 0.0f;
    m_shakeDuration = duration;
}

void Camera::setCustomUpdateFunction(const std::function<void(Camera&, float)>& func) {
    m_customUpdateFunc = func;
}

void Camera::update(float deltaTime) {
    if (m_customUpdateFunc) {
        m_customUpdateFunc(*this, deltaTime);
        return;
    }

    switch (m_mode) {
    case CameraMode::Follow:
        updateFollow(deltaTime);
        break;
    case CameraMode::Fixed:
        updateFixed(deltaTime);
        break;
    case CameraMode::Lerp:
        updateLerp(deltaTime);
        break;
    case CameraMode::LookAhead:
        updateLookAhead(deltaTime);
        break;
    case CameraMode::Rails:
        updateRails(deltaTime);
        break;
    }

    updateShake(deltaTime);

    if (m_constrainToLevel) {
        constrainToLevelBounds();
    }

    m_view.setCenter(m_position);
}

void Camera::applyTo(sf::RenderWindow& window) {
    window.setView(m_view);
}

sf::Vector2f Camera::worldToScreenCoords(const sf::Vector2f& worldCoords, const sf::RenderWindow& window) const {
    sf::Vector2i pixelCoords = window.mapCoordsToPixel(worldCoords, m_view);
    return sf::Vector2f(static_cast<float>(pixelCoords.x), static_cast<float>(pixelCoords.y));
}

sf::Vector2f Camera::screenToWorldCoords(const sf::Vector2f& screenCoords, const sf::RenderWindow& window) const {
    return window.mapPixelToCoords(sf::Vector2i(static_cast<int>(screenCoords.x), static_cast<int>(screenCoords.y)), m_view);
}

const sf::View& Camera::getView() const {
    return m_view;
}

sf::Vector2f Camera::getPosition() const {
    return m_position;
}

float Camera::getZoom() const {
    return m_zoomLevel;
}

float Camera::getRotation() const {
    return m_rotationAngle;
}

Entity* Camera::getTarget() const {
    return m_target;
}

CameraMode Camera::getMode() const {
    return m_mode;
}

sf::Vector2f Camera::getCenter() const {
    return m_view.getCenter();
}

sf::Vector2f Camera::getSize() const {
    return m_view.getSize();
}

sf::Vector2f Camera::getOffset() const {
    return m_offset;
}

float Camera::getLerpFactor() const {
    return m_lerpFactor;
}

float Camera::getLookAheadDistance() const {
    return m_lookAheadDistance;
}

bool Camera::isConstrainedToLevel() const {
    return m_constrainToLevel;
}

void Camera::updateFollow(float dt) {
    if (!m_target) return;

    m_position = m_target->getPosition() + m_offset;
}

void Camera::updateFixed(float dt) {
}

void Camera::updateLerp(float dt) {
    sf::Vector2f targetPos;

    if (m_target) {
        targetPos = m_target->getPosition() + m_offset;
    }
    else {
        targetPos = m_targetPosition;
    }

    float lerpAmount = 1.0f - std::exp(-m_lerpFactor * dt);
    m_position.x = m_position.x + (targetPos.x - m_position.x) * lerpAmount;
    m_position.y = m_position.y + (targetPos.y - m_position.y) * lerpAmount;
}

void Camera::updateLookAhead(float dt) {
    if (!m_target) return;

    sf::Vector2f currentTargetPos = m_target->getPosition();
    sf::Vector2f velocity;

    if (dt > 0.0f) {
        velocity = (currentTargetPos - m_lastTargetPos) / dt;
    }
    else {
        velocity = sf::Vector2f(0.0f, 0.0f);
    }

    float length = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (length > 1.0f) {
        velocity /= length;
    }

    m_predictedPos = currentTargetPos + velocity * m_lookAheadDistance;

    float lerpAmount = 1.0f - std::exp(-m_lerpFactor * dt);
    m_position.x = m_position.x + (m_predictedPos.x - m_position.x) * lerpAmount;
    m_position.y = m_position.y + (m_predictedPos.y - m_position.y) * lerpAmount;

    m_position += m_offset;

    m_lastTargetPos = currentTargetPos;
}

void Camera::updateRails(float dt) {
    if (m_rails.size() < 2) return;

    if (m_target) {

        float minDist = std::numeric_limits<float>::max();
        int closestSegment = 0;
        sf::Vector2f closestPoint;
        sf::Vector2f targetPos = m_target->getPosition();

        for (size_t i = 0; i < m_rails.size() - 1; ++i) {
            sf::Vector2f segStart = m_rails[i];
            sf::Vector2f segEnd = m_rails[i + 1];
            sf::Vector2f segDir = segEnd - segStart;
            float segLength = std::sqrt(segDir.x * segDir.x + segDir.y * segDir.y);

            if (segLength < 0.0001f) continue;

            segDir /= segLength;

            sf::Vector2f toTarget = targetPos - segStart;
            float dot = toTarget.x * segDir.x + toTarget.y * segDir.y;
            dot = std::max(0.0f, std::min(segLength, dot));

            sf::Vector2f pointOnSeg = segStart + segDir * dot;
            sf::Vector2f diff = targetPos - pointOnSeg;
            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

            if (dist < minDist) {
                minDist = dist;
                closestSegment = i;
                closestPoint = pointOnSeg;

                m_railProgress = 0.0f;
                for (int j = 0; j < i; ++j) {
                    sf::Vector2f segDiff = m_rails[j + 1] - m_rails[j];
                    float segLen = std::sqrt(segDiff.x * segDiff.x + segDiff.y * segDiff.y);
                    m_railProgress += segLen;
                }
                m_railProgress += dot;

                float totalLength = 0.0f;
                for (size_t j = 0; j < m_rails.size() - 1; ++j) {
                    sf::Vector2f segDiff = m_rails[j + 1] - m_rails[j];
                    totalLength += std::sqrt(segDiff.x * segDiff.x + segDiff.y * segDiff.y);
                }

                if (totalLength > 0.0f) {
                    m_railProgress /= totalLength;
                }
            }
        }

        float lerpAmount = 1.0f - std::exp(-m_lerpFactor * dt);
        m_position.x = m_position.x + (closestPoint.x - m_position.x) * lerpAmount;
        m_position.y = m_position.y + (closestPoint.y - m_position.y) * lerpAmount;

        m_position += m_offset;
    }
}

void Camera::updateShake(float dt) {
    if (m_shakeTimer < m_shakeDuration) {
        m_shakeTimer += dt;

        float currentIntensity = m_shakeIntensity * (1.0f - m_shakeTimer / m_shakeDuration);

        float offsetX = ((rand() % 200) / 100.0f - 1.0f) * currentIntensity;
        float offsetY = ((rand() % 200) / 100.0f - 1.0f) * currentIntensity;

        m_position.x += offsetX;
        m_position.y += offsetY;
    }
}

void Camera::constrainToLevelBounds() {
    float viewWidthHalf = m_view.getSize().x / 2.0f;
    float viewHeightHalf = m_view.getSize().y / 2.0f;

    if (m_position.x - viewWidthHalf < 0.0f) {
        m_position.x = viewWidthHalf;
    }
    else if (m_position.x + viewWidthHalf > m_bounds.x) {
        m_position.x = m_bounds.x - viewWidthHalf;
    }

    if (m_position.y - viewHeightHalf < 0.0f) {
        m_position.y = viewHeightHalf;
    }
    else if (m_position.y + viewHeightHalf > m_bounds.y) {
        m_position.y = m_bounds.y - viewHeightHalf;
    }
}