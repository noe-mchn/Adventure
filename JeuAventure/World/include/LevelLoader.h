#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

class Level;
class Entity;

// Utilisation de nlohmann json pour le parsing
using json = nlohmann::json;

class LevelLoader {
public:
    // Charge un niveau � partir d'un fichier JSON LDtk
    static bool loadLevel(const std::string& jsonFilePath, Level* level);

private:
    // Structure pour stocker les informations d'une tuile
    struct TileInfo {
        int tileId;
        int x, y;
        bool flipped;
        bool flipY;    // Ajout pour supporter le flip vertical
        bool rotated;
    };

    // Charge les collisions depuis une IntGrid layer
    static void loadCollisionsFromIntGrid(Level* level, const json& layerData, int gridSize);

    // Charge une couche de tuiles
    static void loadTileLayer(Level* level, const json& layerData, const json& project, const std::string& basePath);

    // Charge les entit�s
    static void loadEntities(Level* level, const json& layerData);

    // Cherche le tileset par ID
    static json findTileset(const json& project, int tilesetId);

    // D�code les tuiles d'un layer (format de compression LDtk)
    static std::vector<TileInfo> decodeTiles(const json& layerData, int gridSize);

    // Obtient le niveau courant � partir du projet
    static json getCurrentLevel(const json& project);

    // Cr�e une entit� bas�e sur son type
    static Entity* createEntityByType(const std::string& type, const json& entityData);

    // Cr�e des entit�s par d�faut pour tester
    static void createDefaultEntities(Level* level);
};