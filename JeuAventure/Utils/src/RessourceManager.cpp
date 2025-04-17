#include "RessourceManager.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

RessourceManager* RessourceManager::s_instance = nullptr;

RessourceManager::RessourceManager() {
    initialize();
}

RessourceManager::~RessourceManager() {
    clearAll();
}

RessourceManager* RessourceManager::getInstance() {
    if (!s_instance) {
        s_instance = new RessourceManager();
    }
    return s_instance;
}

bool RessourceManager::initialize() {
    std::filesystem::path execFilePath;

#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    execFilePath = std::filesystem::path(path);
#else
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    execFilePath = std::filesystem::path(std::string(path, (count > 0) ? count : 0));
#endif

    m_executablePath = execFilePath;

    try {
        m_resourceBasePath = execFilePath
            .parent_path()
            .parent_path()
            .parent_path()
            .parent_path()
            .parent_path() / "Ressources";

        if (!std::filesystem::exists(m_resourceBasePath)) {
            std::cerr << "ERREUR: Le dossier de ressources n'existe pas: "
                << m_resourceBasePath << std::endl;

            std::filesystem::path altPath = execFilePath.parent_path() / "Ressources";
            if (std::filesystem::exists(altPath)) {
                std::cout << "Utilisation du chemin alternatif: " << altPath << std::endl;
                m_resourceBasePath = altPath;
            }
            else {
                altPath = std::filesystem::current_path() / "Ressources";
                if (std::filesystem::exists(altPath)) {
                    std::cout << "Utilisation du chemin alternatif: " << altPath << std::endl;
                    m_resourceBasePath = altPath;
                }
                else {
                    std::cerr << "Impossible de trouver le dossier Ressources" << std::endl;
                    return false;
                }
            }
        }

        std::cout << "Chemin d'exécutable: " << m_executablePath << std::endl;
        std::cout << "Chemin des ressources: " << m_resourceBasePath << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur d'initialisation des chemins: " << e.what() << std::endl;
        return false;
    }
}

std::filesystem::path RessourceManager::getResourcePath(const std::string& relativePath) const {
    std::filesystem::path fullPath = m_resourceBasePath / relativePath;

    if (!std::filesystem::exists(fullPath)) {
        std::cerr << "AVERTISSEMENT: Ressource introuvable: " << fullPath << std::endl;
    }

    return fullPath;
}

bool RessourceManager::loadTexture(const std::string& id, const std::string& filename) {
    if (m_textures.find(id) != m_textures.end()) {
        return true;
    }

    auto filepath = getResourcePath(filename);

    if (!std::filesystem::exists(filepath)) {
        if (filename.find("Level_") != std::string::npos) {
            filepath = getResourcePath(filename);
        }

        if (!std::filesystem::exists(filepath)) {
            filepath = filename;
        }
    }

    std::unique_ptr<sf::Texture> texture = std::make_unique<sf::Texture>();

    if (!texture->loadFromFile(filepath.string())) {
        std::cerr << "Échec du chargement de la texture: " << filepath << std::endl;
        return false;
    }

    std::cout << "Texture chargée avec succès: " << id << " (" << filepath << ")" << std::endl;

    m_textures[id] = std::move(texture);
    return true;
}

sf::Texture* RessourceManager::getTexture(const std::string& id) {
    auto it = m_textures.find(id);
    if (it != m_textures.end()) {
        return it->second.get();
    }

    std::cerr << "Texture introuvable: " << id << std::endl;
    return nullptr;
}


bool RessourceManager::loadFont(const std::string& id, const std::string& filename) {
    if (m_fonts.find(id) != m_fonts.end()) {
        return true;
    }

    auto filepath = getResourcePath(filename);

    if (!std::filesystem::exists(filepath)) {
        filepath = filename;
    }

    std::unique_ptr<sf::Font> font = std::make_unique<sf::Font>();

    if (!font->loadFromFile(filepath.string())) {
        std::cerr << "Échec du chargement de la police: " << filepath << std::endl;
        return false;
    }

    std::cout << "Police chargée avec succès: " << id << " (" << filepath << ")" << std::endl;

    m_fonts[id] = std::move(font);
    return true;
}

sf::Font* RessourceManager::getFont(const std::string& id) {
    auto it = m_fonts.find(id);
    if (it != m_fonts.end()) {
        return it->second.get();
    }

    std::cerr << "Police introuvable: " << id << std::endl;
    return nullptr;
}


bool RessourceManager::loadSoundBuffer(const std::string& id, const std::string& filename) {

    if (m_soundBuffers.find(id) != m_soundBuffers.end()) {
        return true;
    }

    auto filepath = getResourcePath("Audio/" + filename);

    if (!std::filesystem::exists(filepath)) {
        filepath = filename;
    }

    std::unique_ptr<sf::SoundBuffer> soundBuffer = std::make_unique<sf::SoundBuffer>();

    if (!soundBuffer->loadFromFile(filepath.string())) {
        std::cerr << "Échec du chargement du son: " << filepath << std::endl;
        return false;
    }

    std::cout << "Son chargé avec succès: " << id << " (" << filepath << ")" << std::endl;

    m_soundBuffers[id] = std::move(soundBuffer);
    return true;
}

sf::SoundBuffer* RessourceManager::getSoundBuffer(const std::string& id) {
    auto it = m_soundBuffers.find(id);
    if (it != m_soundBuffers.end()) {
        return it->second.get();
    }

    std::cerr << "Son introuvable: " << id << std::endl;
    return nullptr;
}


bool RessourceManager::loadMusic(const std::string& id, const std::string& filename) {

    if (m_musics.find(id) != m_musics.end()) {
        return true;
    }

    auto filepath = getResourcePath("Audio/" + filename);

    if (!std::filesystem::exists(filepath)) {
        filepath = filename;
    }

    std::unique_ptr<sf::Music> music = std::make_unique<sf::Music>();

    if (!music->openFromFile(filepath.string())) {
        std::cerr << "Échec du chargement de la musique: " << filepath << std::endl;
        return false;
    }

    std::cout << "Musique chargée avec succès: " << id << " (" << filepath << ")" << std::endl;

    m_musics[id] = std::move(music);
    return true;
}

sf::Music* RessourceManager::getMusic(const std::string& id) {
    auto it = m_musics.find(id);
    if (it != m_musics.end()) {
        return it->second.get();
    }

    std::cerr << "Musique introuvable: " << id << std::endl;
    return nullptr;
}

void RessourceManager::clearAll() {
    m_textures.clear();
    m_fonts.clear();
    m_soundBuffers.clear();
    m_musics.clear();

    std::cout << "Toutes les ressources ont été libérées." << std::endl;
}