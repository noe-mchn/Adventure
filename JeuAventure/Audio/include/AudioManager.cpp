#include "AudioManager.h"
#include <iostream>

AudioManager* AudioManager::instance = nullptr;

AudioManager::AudioManager() :
    masterVolume(1.0f),
    musicVolume(1.0f),
    sfxVolume(1.0f),
    musicDirectory("assets/music/"),
    sfxDirectory("assets/sfx/") {
    musicPlayer = std::make_unique<MusicPlayer>(musicDirectory);
}

AudioManager* AudioManager::getInstance() {
    if (instance == nullptr) {
        instance = new AudioManager();
    }
    return instance;
}

void AudioManager::initialize(const std::string& musicDir, const std::string& sfxDir) {
    musicDirectory = musicDir;
    sfxDirectory = sfxDir;
    musicPlayer->setDirectory(musicDirectory);

    std::cout << "AudioManager initialized" << std::endl;
    std::cout << "Music directory: " << musicDirectory << std::endl;
    std::cout << "SFX directory: " << sfxDirectory << std::endl;
}

void AudioManager::cleanup() {
    stopMusic();
    stopAllSoundEffects();
    soundEffects.clear();
}

void AudioManager::playMusic(const std::string& filename, bool loop) {
    musicPlayer->play(filename, loop);
    musicPlayer->setVolume(masterVolume * musicVolume);
}

void AudioManager::stopMusic() {
    musicPlayer->stop();
}

void AudioManager::pauseMusic() {
    musicPlayer->pause();
}

void AudioManager::resumeMusic() {
    musicPlayer->resume();
}

void AudioManager::loadSoundEffect(const std::string& name, const std::string& filename) {
    std::string fullPath = sfxDirectory + filename;
    auto sound = std::make_shared<SoundEffect>(name, fullPath);
    sound->setVolume(masterVolume * sfxVolume);
    soundEffects[name] = sound;
}

void AudioManager::playSoundEffect(const std::string& name) {
    auto it = soundEffects.find(name);
    if (it != soundEffects.end()) {
        for (auto& sfx : soundEffects) {
            if (sfx.first != name) {
                sfx.second->reduceVolume();
            }
        }

        it->second->play();
    }
    else {
        std::cerr << "Sound effect not found: " << name << std::endl;
    }
}

void AudioManager::stopSoundEffect(const std::string& name) {
    auto it = soundEffects.find(name);
    if (it != soundEffects.end()) {
        it->second->stop();

        for (auto& sfx : soundEffects) {
            sfx.second->resetToBaseVolume();
        }
    }
}

void AudioManager::stopAllSoundEffects() {
    for (auto& sfx : soundEffects) {
        sfx.second->stop();
        sfx.second->resetToBaseVolume();
    }
}

void AudioManager::setMasterVolume(float volume) {
    masterVolume = volume;
    musicPlayer->setVolume(masterVolume * musicVolume);

    for (auto& sfx : soundEffects) {
        sfx.second->setVolume(masterVolume * sfxVolume);
    }
}

void AudioManager::setMusicVolume(float volume) {
    musicVolume = volume;
    musicPlayer->setVolume(masterVolume * musicVolume);
}

void AudioManager::setSfxVolume(float volume) {
    sfxVolume = volume;
    for (auto& sfx : soundEffects) {
        sfx.second->setVolume(masterVolume * sfxVolume);
    }
}

float AudioManager::getMasterVolume() const {
    return masterVolume;
}

float AudioManager::getMusicVolume() const {
    return musicVolume;
}

float AudioManager::getSfxVolume() const {
    return sfxVolume;
}

void AudioManager::setMusicDirectory(const std::string& dir) {
    musicDirectory = dir;
    musicPlayer->setDirectory(musicDirectory);
}

void AudioManager::setSfxDirectory(const std::string& dir) {
    sfxDirectory = dir;
}

std::string AudioManager::getMusicDirectory() const {
    return musicDirectory;
}

std::string AudioManager::getSfxDirectory() const {
    return sfxDirectory;
}

void AudioManager::update() {
    bool anySoundPlaying = false;
    for (auto& sfx : soundEffects) {
        if (sfx.second->getIsPlaying()) {
            anySoundPlaying = true;
            break;
        }
    }

    if (!anySoundPlaying) {
        for (auto& sfx : soundEffects) {
            sfx.second->resetToBaseVolume();
        }
    }
}

AudioManager::~AudioManager() {
    cleanup();
}