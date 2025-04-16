#pragma once

#include <SFML/Audio.hpp>
#include <string>
#include <memory>

class SoundEffect {
private:
    sf::SoundBuffer buffer;
    sf::Sound sound;
    std::string name;
    float baseVolume;
    bool isPlaying;

public:
    SoundEffect(const std::string& name, const std::string& filepath);

    void play();
    void stop();

    void setVolume(float volume);
    float getVolume() const;
    void resetToBaseVolume();
    void reduceVolume();

    bool getIsPlaying() const;
    std::string getName() const;

    ~SoundEffect();
};