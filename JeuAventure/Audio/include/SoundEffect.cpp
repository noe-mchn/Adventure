#include "SoundEffect.h"
#include <iostream>

SoundEffect::SoundEffect(const std::string& soundName, const std::string& filepath)
    : name(soundName), baseVolume(1.0f), isPlaying(false) {

    if (!buffer.loadFromFile(filepath)) {
        std::cerr << "Failed to load sound file: " << filepath << std::endl;
    }
    else {
        sound.setBuffer(buffer);
    }
}

void SoundEffect::play() {
    sound.play();
    isPlaying = true;
}

void SoundEffect::stop() {
    sound.stop();
    isPlaying = false;
}

void SoundEffect::setVolume(float volume) {
    baseVolume = volume;
    sound.setVolume(baseVolume * 100);
}

float SoundEffect::getVolume() const {
    return baseVolume;
}

void SoundEffect::resetToBaseVolume() {
    sound.setVolume(baseVolume * 100);
}

void SoundEffect::reduceVolume() {
    sound.setVolume(baseVolume * 50);
}

bool SoundEffect::getIsPlaying() const {
    return (sound.getStatus() == sf::Sound::Playing);
}

std::string SoundEffect::getName() const {
    return name;
}

SoundEffect::~SoundEffect() {
    stop();
}