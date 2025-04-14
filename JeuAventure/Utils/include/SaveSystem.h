#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <SFML/System/Vector2.hpp>

class SaveSystem {
private:
    static SaveSystem* s_instance;

    std::map<std::string, int> m_intValues;
    std::map<std::string, float> m_floatValues;
    std::map<std::string, bool> m_boolValues;
    std::map<std::string, std::string> m_stringValues;
    std::map<std::string, sf::Vector2f> m_vector2fValues;

    std::string m_savePath;
    bool m_hasUnsavedChanges;

    SaveSystem();

public:
    SaveSystem(const SaveSystem&) = delete;
    SaveSystem& operator=(const SaveSystem&) = delete;

    static SaveSystem* getInstance();

    static void cleanup();

    void setSavePath(const std::string& path);

    bool saveToFile(const std::string& fileName);
    bool loadFromFile(const std::string& fileName);

    void setInt(const std::string& key, int value);
    int getInt(const std::string& key, int defaultValue = 0);

    void setFloat(const std::string& key, float value);
    float getFloat(const std::string& key, float defaultValue = 0.0f);

    void setBool(const std::string& key, bool value);
    bool getBool(const std::string& key, bool defaultValue = false);

    void setString(const std::string& key, const std::string& value);
    std::string getString(const std::string& key, const std::string& defaultValue = "");

    void setVector2f(const std::string& key, const sf::Vector2f& value);
    sf::Vector2f getVector2f(const std::string& key, const sf::Vector2f& defaultValue = sf::Vector2f(0, 0));

    bool saveExists(const std::string& fileName);

    void clearData();
};
