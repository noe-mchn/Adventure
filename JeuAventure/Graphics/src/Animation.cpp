#include "Animation.h"
#include <iostream>

Animation::Animation() :
    m_currentFrame(0),
    m_timer(0.0f),
    m_isLooping(true),
    m_isFinished(false),
    m_speedFactor(1.0f),
    m_defaultRect(0, 0, 0, 0),
    m_flipHorizontal(false)
{
}

void Animation::addFrame(const sf::IntRect& rect, float duration) {
    AnimationFrame frame;
    frame.rect = rect;
    frame.duration = duration;
    m_frames.push_back(frame);

    if (m_frames.size() == 1) {
        m_defaultRect = rect;
    }
}

void Animation::addGridFrames(const sf::Vector2i& frameSize, const sf::Vector2i& startPos,
    unsigned int count, float frameDuration,
    unsigned int columns, unsigned int spacing) {
    if (count == 0) return;

    if (columns == 0) {
        columns = count;
    }

    for (unsigned int i = 0; i < count; ++i) {
        int x = startPos.x + (i % columns) * (frameSize.x + spacing);
        int y = startPos.y + (i / columns) * (frameSize.y + spacing);

        addFrame(sf::IntRect(x, y, frameSize.x, frameSize.y), frameDuration);
    }
}

void Animation::update(float deltaTime) {
    if (m_frames.empty() || (m_isFinished && !m_isLooping)) {
        return;
    }

    m_timer += deltaTime * m_speedFactor;

    while (m_timer >= m_frames[m_currentFrame].duration && !m_isFinished) {
        m_timer -= m_frames[m_currentFrame].duration;
        m_currentFrame++;

        if (m_currentFrame >= m_frames.size()) {
            if (m_isLooping) {
                m_currentFrame = 0;
            }
            else {
                m_currentFrame = m_frames.size() - 1;
                m_isFinished = true;
            }
        }
    }
}

void Animation::apply(sf::Sprite& sprite) {
    if (m_frames.empty()) {
        sprite.setTextureRect(m_defaultRect);
        return;
    }

    const sf::IntRect& rect = m_frames[m_currentFrame].rect;

    if (m_flipHorizontal) {
        sf::IntRect flippedRect = rect;
        flippedRect.left = flippedRect.left + flippedRect.width;
        flippedRect.width = -flippedRect.width;
        sprite.setTextureRect(flippedRect);
    }
    else {
        sprite.setTextureRect(rect);
    }
}

void Animation::reset() {
    m_currentFrame = 0;
    m_timer = 0.0f;
    m_isFinished = false;
}

void Animation::setLooping(bool looping) {
    m_isLooping = looping;
}

bool Animation::isLooping() const {
    return m_isLooping;
}

bool Animation::isFinished() const {
    return m_isFinished;
}

void Animation::setSpeed(float speed) {
    m_speedFactor = speed;
}

float Animation::getSpeed() const {
    return m_speedFactor;
}

void Animation::setFlipHorizontal(bool flip) {
    m_flipHorizontal = flip;
}

bool Animation::getFlipHorizontal() const {
    return m_flipHorizontal;
}

void Animation::setCurrentFrame(unsigned int frame) {
    if (frame < m_frames.size()) {
        m_currentFrame = frame;
        m_timer = 0.0f;
    }
}

unsigned int Animation::getCurrentFrame() const {
    return m_currentFrame;
}

unsigned int Animation::getFrameCount() const {
    return static_cast<unsigned int>(m_frames.size());
}

void Animation::setDefaultRect(const sf::IntRect& rect) {
    m_defaultRect = rect;
}

const sf::IntRect& Animation::getDefaultRect() const {
    return m_defaultRect;
}

bool Animation::isValid() const {
    return !m_frames.empty();
}