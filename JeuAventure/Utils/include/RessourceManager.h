#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <memory>
#include <string>

class RessourceManager {
private:
    static RessourceManager* s_instance;

    std::map<std::string, std::unique_ptr<sf::Texture>> m_textures;
    std::map<std::string, std::unique_ptr<sf::SoundBuffer>> m_soundBuffers;
    std::map<std::string, std::unique_ptr<sf::Font>> m_fonts;

    RessourceManager() = default;

public:
    RessourceManager(const RessourceManager&) = delete;
    RessourceManager& operator=(const RessourceManager&) = delete;

    static RessourceManager* getInstance();

    static void cleanup();

    bool loadTexture(const std::string& id, const std::string& filePath);
    sf::Texture* getTexture(const std::string& id);

    bool loadSoundBuffer(const std::string& id, const std::string& filePath);
    sf::SoundBuffer* getSoundBuffer(const std::string& id);

    bool loadFont(const std::string& id, const std::string& filePath);
    sf::Font* getFont(const std::string& id);
};
