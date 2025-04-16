#include "MusicPlayer.h"
#include <iostream>

MusicPlayer::MusicPlayer(const std::string& dir) : volume(1.0f), isPlaying(false), directory(dir) {
}

void MusicPlayer::play(const std::string& filename, bool loop) {
    stop();

    std::string fullPath = directory + filename;

    if (music.openFromFile(fullPath)) {
        currentTrack = filename;
        music.setLoop(loop);
        music.setVolume(volume * 100);
        music.play();
        isPlaying = true;
    }
    else {
        std::cerr << "Failed to load music file: " << fullPath << std::endl;
    }
}

void MusicPlayer::stop() {
    if (music.getStatus() != sf::Music::Stopped) {
        music.stop();
        isPlaying = false;
    }
}

void MusicPlayer::pause() {
    if (music.getStatus() == sf::Music::Playing) {
        music.pause();
        isPlaying = false;
    }
}

void MusicPlayer::resume() {
    if (music.getStatus() == sf::Music::Paused) {
        music.play();
        isPlaying = true;
    }
}

void MusicPlayer::setVolume(float vol) {
    volume = vol;
    music.setVolume(volume * 100);
}

float MusicPlayer::getVolume() const {
    return volume;
}

void MusicPlayer::setDirectory(const std::string& dir) {
    directory = dir;
}

std::string MusicPlayer::getDirectory() const {
    return directory;
}

bool MusicPlayer::getIsPlaying() const {
    return isPlaying;
}

std::string MusicPlayer::getCurrentTrack() const {
    return currentTrack;
}

MusicPlayer::~MusicPlayer() {
    stop();
}