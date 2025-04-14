#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct AnimationFrame {
    sf::IntRect rect;
    float duration;
};

class Animation {
private:
    std::vector<AnimationFrame> m_frames;
    unsigned int m_currentFrame;
    float m_timer;
    bool m_isLooping;
    bool m_isFinished;
    float m_speedFactor;
    sf::IntRect m_defaultRect;
    bool m_flipHorizontal;

public:
    Animation();

    void addFrame(const sf::IntRect& rect, float duration);

    void addGridFrames(const sf::Vector2i& frameSize, const sf::Vector2i& startPos,
        unsigned int count, float frameDuration,
        unsigned int columns = 0, unsigned int spacing = 0);

    void update(float deltaTime);

    void apply(sf::Sprite& sprite);

    void reset();

    void setLooping(bool looping);
    bool isLooping() const;

    bool isFinished() const;

    void setSpeed(float speed);
    float getSpeed() const;

    void setFlipHorizontal(bool flip);
    bool getFlipHorizontal() const;

    void setCurrentFrame(unsigned int frame);
    unsigned int getCurrentFrame() const;

    unsigned int getFrameCount() const;

    void setDefaultRect(const sf::IntRect& rect);
    const sf::IntRect& getDefaultRect() const;

    bool isValid() const;
};
