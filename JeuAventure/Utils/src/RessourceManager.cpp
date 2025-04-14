#include "RessourceManager.h"
#include <iostream>

RessourceManager* RessourceManager::s_instance = nullptr;

RessourceManager* RessourceManager::getInstance() {
    if (s_instance == nullptr) {
        s_instance = new RessourceManager();
    }
    return s_instance;
}

void RessourceManager::cleanup() {
    if (s_instance != nullptr) {
        delete s_instance;
        s_instance = nullptr;
    }
}

bool RessourceManager::loadTexture(const std::string& id, const std::string& filePath) {
    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(filePath)) {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
        return false;
    }

    m_textures[id] = std::move(texture);
    return true;
}

sf::Texture* RessourceManager::getTexture(const std::string& id) {
    auto it = m_textures.find(id);
    if (it != m_textures.end()) {
        return it->second.get();
    }

    std::cerr << "Texture not found: " << id << std::endl;
    return nullptr;
}

bool RessourceManager::loadSoundBuffer(const std::string& id, const std::string& filePath) {
    auto soundBuffer = std::make_unique<sf::SoundBuffer>();
    if (!soundBuffer->loadFromFile(filePath)) {
        std::cerr << "Failed to load sound: " << filePath << std::endl;
        return false;
    }

    m_soundBuffers[id] = std::move(soundBuffer);
    return true;
}

sf::SoundBuffer* RessourceManager::getSoundBuffer(const std::string& id) {
    auto it = m_soundBuffers.find(id);
    if (it != m_soundBuffers.end()) {
        return it->second.get();
    }

    std::cerr << "Sound buffer not found: " << id << std::endl;
    return nullptr;
}

bool RessourceManager::loadFont(const std::string& id, const std::string& filePath) {
    auto font = std::make_unique<sf::Font>();
    if (!font->loadFromFile(filePath)) {
        std::cerr << "Failed to load font: " << filePath << std::endl;
        return false;
    }

    m_fonts[id] = std::move(font);
    return true;
}

sf::Font* RessourceManager::getFont(const std::string& id) {
    auto it = m_fonts.find(id);
    if (it != m_fonts.end()) {
        return it->second.get();
    }

    std::cerr << "Font not found: " << id << std::endl;
    return nullptr;
}