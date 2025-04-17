#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

class Level;
class Entity;

using json = nlohmann::json;

class LevelLoader {
public:
    static bool loadLevel(const std::string& jsonFilePath, Level* level);

private:
    struct TileInfo {
        int tileId;
        int x, y;
        bool flipped;
        bool flipY;
        bool rotated;
    };

    static void loadCollisionsFromIntGrid(Level* level, const json& layerData, int gridSize);
    static void loadTileLayer(Level* level, const json& layerData, const json& project, const std::string& basePath);
    static void loadEntities(Level* level, const json& layerData);
    static json findTileset(const json& project, int tilesetId);
    static std::vector<TileInfo> decodeTiles(const json& layerData, int gridSize);
    static json getCurrentLevel(const json& project);
    static Entity* createEntityByType(const std::string& type, const json& entityData);
    static void createDefaultEntities(Level* level);
};