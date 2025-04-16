#pragma once

#include <SFML/Audio.hpp>
#include <string>

class MusicPlayer {
private:
    sf::Music music;
    std::string currentTrack;
    float volume;
    bool isPlaying;
    std::string directory;

public:
    MusicPlayer(const std::string& dir = "");

    void play(const std::string& filename, bool loop = true);
    void stop();
    void pause();
    void resume();

    void setVolume(float volume);
    float getVolume() const;

    void setDirectory(const std::string& dir);
    std::string getDirectory() const;

    bool getIsPlaying() const;
    std::string getCurrentTrack() const;

    ~MusicPlayer();
};