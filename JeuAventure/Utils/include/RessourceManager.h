#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <filesystem>

class RessourceManager {
private:
    static RessourceManager* s_instance;

    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textures;
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> m_fonts;
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> m_soundBuffers;
    std::unordered_map<std::string, std::unique_ptr<sf::Music>> m_musics;

    std::filesystem::path m_executablePath;
    std::filesystem::path m_resourceBasePath;

    RessourceManager();

public:
    ~RessourceManager();

    RessourceManager(const RessourceManager&) = delete;
    RessourceManager& operator=(const RessourceManager&) = delete;

    static RessourceManager* getInstance();

    bool initialize();

    bool loadTexture(const std::string& id, const std::string& filename);
    sf::Texture* getTexture(const std::string& id);

    bool loadFont(const std::string& id, const std::string& filename);
    sf::Font* getFont(const std::string& id);

    bool loadSoundBuffer(const std::string& id, const std::string& filename);
    sf::SoundBuffer* getSoundBuffer(const std::string& id);

    bool loadMusic(const std::string& id, const std::string& filename);
    sf::Music* getMusic(const std::string& id);

    std::filesystem::path getResourcePath(const std::string& relativePath) const;
    void clearAll();
};