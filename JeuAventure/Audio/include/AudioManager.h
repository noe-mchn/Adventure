#pragma once

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include "SoundEffect.h"
#include "MusicPlayer.h"

class AudioManager {
private:
    static AudioManager* instance;

    std::unordered_map<std::string, std::shared_ptr<SoundEffect>> soundEffects;
    std::unique_ptr<MusicPlayer> musicPlayer;

    float masterVolume;
    float musicVolume;
    float sfxVolume;

    std::string musicDirectory;
    std::string sfxDirectory;

    AudioManager();

public:
    static AudioManager* getInstance();

    void initialize(const std::string& musicDir = "assets/music/", const std::string& sfxDir = "assets/sfx/");
    void cleanup();

    void playMusic(const std::string& filename, bool loop = true);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();

    void loadSoundEffect(const std::string& name, const std::string& filename);
    void playSoundEffect(const std::string& name);
    void stopSoundEffect(const std::string& name);
    void stopAllSoundEffects();

    void setMasterVolume(float volume);
    void setMusicVolume(float volume);
    void setSfxVolume(float volume);

    float getMasterVolume() const;
    float getMusicVolume() const;
    float getSfxVolume() const;

    void setMusicDirectory(const std::string& dir);
    void setSfxDirectory(const std::string& dir);
    std::string getMusicDirectory() const;
    std::string getSfxDirectory() const;

    void update();

    ~AudioManager();
};