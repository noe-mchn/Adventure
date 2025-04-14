#pragma once

#include <string>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

class Level;
namespace nlohmann {
    class json;
}

struct TileData {
    int id;
    int x;
    int y;
    bool collidable;
    std::string type;
    std::vector<std::string> properties;
};

struct EntityData {
    std::string type;
    std::string name;
    float x;
    float y;
    std::vector<std::pair<std::string, std::string>> properties;
};

struct LayerData {
    std::string name;
    std::vector<TileData> tiles;
    std::vector<EntityData> entities;
    bool visible;
};

struct LevelData {
    int width;
    int height;
    int tileWidth;
    int tileHeight;
    std::string backgroundPath;
    std::vector<LayerData> layers;
    std::vector<std::pair<std::string, std::string>> properties;
    std::string musicPath;
};

class LevelLoader {
public:
    static bool loadFromFile(const std::string& filename, Level* level);
    static bool loadFromJson(const std::string& jsonString, Level* level);

private:
    static LevelData parseJson(const nlohmann::json& jsonData);
    static std::vector<TileData> parseTiles(const nlohmann::json& tilesJson);
    static std::vector<EntityData> parseEntities(const nlohmann::json& entitiesJson);
    static std::vector<std::pair<std::string, std::string>> parseProperties(const nlohmann::json& propertiesJson);
    static void applyLevelData(Level* level, const LevelData& levelData);
};
