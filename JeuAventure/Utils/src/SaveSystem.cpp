#include "SaveSystem.h"
#include <iostream>
#include <filesystem>
#include <sstream>
#include <fstream>

namespace fs = std::filesystem;

SaveSystem* SaveSystem::s_instance = nullptr;

SaveSystem::SaveSystem() : m_hasUnsavedChanges(false) {
    m_savePath = "./saves/";
}

SaveSystem* SaveSystem::getInstance() {
    if (s_instance == nullptr) {
        s_instance = new SaveSystem();
    }
    return s_instance;
}

void SaveSystem::cleanup() {
    if (s_instance != nullptr) {
        delete s_instance;
        s_instance = nullptr;
    }
}

void SaveSystem::setSavePath(const std::string& path) {
    m_savePath = path;

    if (!fs::exists(m_savePath)) {
        try {
            fs::create_directories(m_savePath);
        }
        catch (const std::exception& e) {
            std::cerr << "Could not create save directory: " << e.what() << std::endl;
        }
    }
}

bool SaveSystem::saveToFile(const std::string& fileName) {
    std::string fullPath = m_savePath + fileName;
    std::ofstream file(fullPath);

    if (!file.is_open()) {
        std::cerr << "Could not open file for saving: " << fullPath << std::endl;
        return false;
    }

    file << "[Integers]" << std::endl;
    for (const auto& [key, value] : m_intValues) {
        file << key << "=" << value << std::endl;
    }

    file << "[Floats]" << std::endl;
    for (const auto& [key, value] : m_floatValues) {
        file << key << "=" << value << std::endl;
    }

    file << "[Booleans]" << std::endl;
    for (const auto& [key, value] : m_boolValues) {
        file << key << "=" << (value ? "true" : "false") << std::endl;
    }

    file << "[Strings]" << std::endl;
    for (const auto& [key, value] : m_stringValues) {
        file << key << "=" << value << std::endl;
    }

    file << "[Vector2f]" << std::endl;
    for (const auto& [key, value] : m_vector2fValues) {
        file << key << "=" << value.x << "," << value.y << std::endl;
    }

    file.close();
    m_hasUnsavedChanges = false;
    return true;
}

bool SaveSystem::loadFromFile(const std::string& fileName) {
    std::string fullPath = m_savePath + fileName;
    std::ifstream file(fullPath);

    if (!file.is_open()) {
        std::cerr << "Could not open file for loading: " << fullPath << std::endl;
        return false;
    }

    clearData();

    std::string line;
    std::string section;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line[0] == '[' && line.back() == ']') {
            section = line.substr(1, line.size() - 2);
            continue;
        }

        size_t separator = line.find('=');
        if (separator == std::string::npos) continue;

        std::string key = line.substr(0, separator);
        std::string value = line.substr(separator + 1);

        if (section == "Integers") {
            try {
                m_intValues[key] = std::stoi(value);
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing int value: " << e.what() << std::endl;
            }
        }
        else if (section == "Floats") {
            try {
                m_floatValues[key] = std::stof(value);
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing float value: " << e.what() << std::endl;
            }
        }
        else if (section == "Booleans") {
            m_boolValues[key] = (value == "true");
        }
        else if (section == "Strings") {
            m_stringValues[key] = value;
        }
        else if (section == "Vector2f") {
            size_t comma = value.find(',');
            if (comma != std::string::npos) {
                try {
                    float x = std::stof(value.substr(0, comma));
                    float y = std::stof(value.substr(comma + 1));
                    m_vector2fValues[key] = sf::Vector2f(x, y);
                }
                catch (const std::exception& e) {
                    std::cerr << "Error parsing Vector2f value: " << e.what() << std::endl;
                }
            }
        }
    }

    file.close();
    m_hasUnsavedChanges = false;
    return true;
}

void SaveSystem::setInt(const std::string& key, int value) {
    m_intValues[key] = value;
    m_hasUnsavedChanges = true;
}

int SaveSystem::getInt(const std::string& key, int defaultValue) {
    auto it = m_intValues.find(key);
    if (it != m_intValues.end()) {
        return it->second;
    }
    return defaultValue;
}

void SaveSystem::setFloat(const std::string& key, float value) {
    m_floatValues[key] = value;
    m_hasUnsavedChanges = true;
}

float SaveSystem::getFloat(const std::string& key, float defaultValue) {
    auto it = m_floatValues.find(key);
    if (it != m_floatValues.end()) {
        return it->second;
    }
    return defaultValue;
}

void SaveSystem::setBool(const std::string& key, bool value) {
    m_boolValues[key] = value;
    m_hasUnsavedChanges = true;
}

bool SaveSystem::getBool(const std::string& key, bool defaultValue) {
    auto it = m_boolValues.find(key);
    if (it != m_boolValues.end()) {
        return it->second;
    }
    return defaultValue;
}

void SaveSystem::setString(const std::string& key, const std::string& value) {
    m_stringValues[key] = value;
    m_hasUnsavedChanges = true;
}

std::string SaveSystem::getString(const std::string& key, const std::string& defaultValue) {
    auto it = m_stringValues.find(key);
    if (it != m_stringValues.end()) {
        return it->second;
    }
    return defaultValue;
}

void SaveSystem::setVector2f(const std::string& key, const sf::Vector2f& value) {
    m_vector2fValues[key] = value;
    m_hasUnsavedChanges = true;
}

sf::Vector2f SaveSystem::getVector2f(const std::string& key, const sf::Vector2f& defaultValue) {
    auto it = m_vector2fValues.find(key);
    if (it != m_vector2fValues.end()) {
        return it->second;
    }
    return defaultValue;
}

bool SaveSystem::saveExists(const std::string& fileName) {
    std::string fullPath = m_savePath + fileName;
    return fs::exists(fullPath);
}

void SaveSystem::clearData() {
    m_intValues.clear();
    m_floatValues.clear();
    m_boolValues.clear();
    m_stringValues.clear();
    m_vector2fValues.clear();
    m_hasUnsavedChanges = false;
}