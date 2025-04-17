#include "LevelLoader.h"
#include "Level.h"
#include "Tilemap.h"
#include "RessourceManager.h"
#include "Player.h"
#include "Objects.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <SFML/Graphics.hpp>

bool LevelLoader::loadLevel(const std::string& jsonFilePath, Level* level) {
    if (!level) {
        std::cerr << "Error: Level pointer is null" << std::endl;
        return false;
    }

    try {
        std::cout << "Loading LDtk level from: " << jsonFilePath << std::endl;

        // Lire le fichier JSON
        std::ifstream file(jsonFilePath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open LDtk file: " << jsonFilePath << std::endl;
            return false;
        }

        json projectData;
        file >> projectData;
        file.close();

        // Obtenir le chemin de base pour les ressources
        std::string basePath = getBasePath(jsonFilePath);

        // Obtenir les informations générales du projet
        int defaultGridSize = projectData["defaultGridSize"];
        int defaultLevelWidth = projectData["defaultLevelWidth"];
        int defaultLevelHeight = projectData["defaultLevelHeight"];

        // Configurer les dimensions du niveau
        level->setSize(defaultLevelWidth, defaultLevelHeight);
        level->setTileSize(defaultGridSize, defaultGridSize);

        // Initialiser la tilemap
        Tilemap* tilemap = level->getTilemap();
        if (!tilemap) {
            tilemap = new Tilemap(defaultLevelWidth / defaultGridSize, defaultLevelHeight / defaultGridSize);
            level->setTilemap(tilemap);
        }
        tilemap->setTileSize(defaultGridSize, defaultGridSize);

        // Obtenir le niveau actuel (pour cet exemple, nous prenons simplement le premier niveau)
        json currentLevel = getCurrentLevel(projectData);

        if (currentLevel.empty()) {
            std::cerr << "Error: No level found in LDtk file" << std::endl;
            return false;
        }

        // Traiter les couches dans l'ordre inverse (de l'arrière vers l'avant)
        auto& layerInstances = currentLevel["layerInstances"];
        for (int i = layerInstances.size() - 1; i >= 0; i--) {
            const auto& layer = layerInstances[i];
            std::string layerType = layer["__type"];
            std::string layerId = layer["__identifier"];

            std::cout << "Processing layer: " << layerId << " (Type: " << layerType << ")" << std::endl;

            if (layerType == "IntGrid") {
                // Charger les collisions pour les couches IntGrid
                loadCollisionsFromIntGrid(level, layer, defaultGridSize);
            }
            else if (layerType == "Tiles") {
                // Charger les tuiles pour l'affichage
                loadTileLayer(level, layer, projectData, basePath);
            }
            else if (layerType == "Entities") {
                // Charger les entités
                loadEntities(level, layer);
            }
        }

        // Définir la position de départ du joueur 
        // (rechercher une entité marquée comme point de départ ou utiliser une position par défaut)
        sf::Vector2f playerStart(100, 100);
        level->setPlayerStart(playerStart);

        // Initialiser le niveau
        level->initialize();

        std::cout << "LDtk level loaded successfully" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading LDtk level: " << e.what() << std::endl;
        return false;
    }
}

void LevelLoader::loadCollisionsFromIntGrid(Level* level, const json& layerData, int gridSize) {
    Tilemap* tilemap = level->getTilemap();
    if (!tilemap) return;

    // Obtenir les dimensions de la grille
    int gridWidth = layerData["__cWid"];
    int gridHeight = layerData["__cHei"];

    // Vérifier si la couche a des valeurs IntGrid
    if (layerData.contains("intGridCsv")) {
        const auto& intGridValues = layerData["intGridCsv"];

        int collisionCount = 0;

        for (int y = 0; y < gridHeight; y++) {
            for (int x = 0; x < gridWidth; x++) {
                int idx = y * gridWidth + x;
                if (idx < intGridValues.size()) {
                    int value = intGridValues[idx];
                    if (value > 0) { // 0 = pas de collision, tout autre valeur = collision
                        tilemap->setTile(x, y, value);
                        tilemap->setTileCollision(x, y, true);
                        collisionCount++;

                        // Si c'est une collision mortelle (value = 2), on peut ajouter un traitement spécial ici
                        if (value == 2) {
                            // Marquer comme collision mortelle (dépend de votre implémentation)
                            // tilemap->setDeadlyCollision(x, y, true);
                        }
                    }
                }
            }
        }

        std::cout << "Added " << collisionCount << " collision tiles from layer " << (std::string)layerData["__identifier"] << std::endl;
    }
}

void LevelLoader::loadTileLayer(Level* level, const json& layerData, const json& project, const std::string& basePath) {
    // Obtenir l'ID du tileset pour cette couche
    int tilesetDefUid = layerData["__tilesetDefUid"];

    // Trouver le tileset dans le projet
    json tileset = findTileset(project, tilesetDefUid);
    if (tileset.empty()) return;

    // Obtenir le chemin relatif de la texture du tileset
    std::string relPath = tileset["relPath"];
    std::string tilesetId = tileset["identifier"];

    // Charger la texture du tileset
    RessourceManager* resourceManager = RessourceManager::getInstance();
    sf::Texture* texture = nullptr;

    if (resourceManager->loadTexture(tilesetId, basePath + relPath)) {
        texture = resourceManager->getTexture(tilesetId);
    }
    else {
        std::cerr << "Failed to load tileset texture: " << basePath + relPath << std::endl;
        return;
    }

    if (!texture) return;

    // Créer un sprite pour cette couche
    auto layerSprite = std::make_unique<sf::RenderTexture>();
    if (!layerSprite->create(level->getWidth(), level->getHeight())) {
        std::cerr << "Failed to create render texture for layer" << std::endl;
        return;
    }
    layerSprite->clear(sf::Color::Transparent);

    // Obtenir les propriétés du tileset
    int tileSize = tileset["tileGridSize"];
    int tilesetWidth = tileset["__cWid"];

    // Décoder les tuiles de la couche
    std::vector<TileInfo> tiles = decodeTiles(layerData, tileSize);

    // Dessiner chaque tuile dans la texture de la couche
    sf::Sprite tileSprite(*texture);

    for (const auto& tile : tiles) {
        // Calculer la position de la tuile dans le tileset
        int srcX = (tile.tileId % tilesetWidth) * tileSize;
        int srcY = (tile.tileId / tilesetWidth) * tileSize;

        // Configurer le rectangle source de la tuile
        tileSprite.setTextureRect(sf::IntRect(srcX, srcY, tileSize, tileSize));
        tileSprite.setPosition(tile.x, tile.y);

        // Gérer le flip/rotation si nécessaire
        if (tile.flipped || tile.rotated) {
            // Implémenter selon les besoins...
        }

        // Dessiner la tuile dans la texture de la couche
        layerSprite->draw(tileSprite);
    }

    layerSprite->display();

    // Créer un sprite final pour cette couche et l'ajouter au niveau
    auto finalSprite = std::make_unique<sf::Sprite>(layerSprite->getTexture());

    // Configurer les propriétés de parallaxe de la couche si nécessaires
    float parallaxFactorX = layerData.value("parallaxFactorX", 1.0f);
    float parallaxFactorY = layerData.value("parallaxFactorY", 1.0f);

    // Cas spécial pour les couches de fond
    std::string layerId = layerData["__identifier"];
    if (layerId == "RealBackground" || layerId == "Mid" || layerId == "Back") {
        if (layerId == "RealBackground") {
            // Définir comme fond principal
            level->setBackground(texture);
        }
        else {
            // Ajouter comme couche parallaxe supplémentaire
            level->addParallaxLayer(finalSprite.get(), sf::Vector2f(parallaxFactorX, parallaxFactorY));
        }
    }
    else {
        // Ajouter comme couche de tuiles normale
        level->addTileLayer(std::move(finalSprite));
    }
}

void LevelLoader::loadEntities(Level* level, const json& layerData) {
    if (!layerData.contains("entityInstances")) return;

    const auto& entities = layerData["entityInstances"];

    for (const auto& entityData : entities) {
        std::string entityType = entityData["__identifier"];

        // Créer l'entité en fonction de son type
        Entity* entity = createEntityByType(entityType, entityData);

        if (entity) {
            // Configurer la position de l'entité
            int pixelX = entityData["__grid"][0] * (int)entityData["__gridSize"];
            int pixelY = entityData["__grid"][1] * (int)entityData["__gridSize"];
            entity->setPosition(pixelX, pixelY);

            // Ajouter l'entité au niveau
            level->addEntity(entity);
        }
    }
}

json LevelLoader::findTileset(const json& project, int tilesetId) {
    if (project.contains("defs") && project["defs"].contains("tilesets")) {
        for (const auto& tileset : project["defs"]["tilesets"]) {
            if (tileset["uid"] == tilesetId) {
                return tileset;
            }
        }
    }
    return json();
}

sf::Texture* LevelLoader::loadTexture(const std::string& basePath, const std::string& relPath) {
    RessourceManager* resourceManager = RessourceManager::getInstance();
    std::string fullPath = basePath + relPath;

    std::string id = relPath;
    size_t lastSlash = relPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        id = relPath.substr(lastSlash + 1);
    }

    if (resourceManager->loadTexture(id, fullPath)) {
        return resourceManager->getTexture(id);
    }

    return nullptr;
}

std::string LevelLoader::getBasePath(const std::string& jsonFilePath) {
    std::filesystem::path path(jsonFilePath);
    return path.parent_path().string() + "/";
}

Entity* LevelLoader::createEntityByType(const std::string& type, const json& entityData) {
    // Par défaut, utiliser une simple entité
    Entity* entity = nullptr;

    // Vérifier le type d'entité et créer l'instance appropriée
    if (type == "Player" || type == "PlayerStart") {
        // Point de spawn du joueur, on ne crée pas d'entité mais on définit le point de départ
        return nullptr;
    }
    else if (type == "Coin" || type == "Bitcoin") {
        entity = new Pickup("Bitcoin", 1);
    }
    else if (type == "Health") {
        entity = new Pickup("health", 20);
    }
    else if (type == "Checkpoint") {
        entity = new Checkpoint();
    }
    else if (type == "Finish" || type == "Exit") {
        Trigger* trigger = new Trigger();
        trigger->setTriggerTag("finish");
        entity = trigger;
    }

    // Configurer la taille et d'autres propriétés communes
    if (entity) {
        int width = entityData.value("width", 16);
        int height = entityData.value("height", 16);
        entity->setSize(sf::Vector2f(width, height));
    }

    return entity;
}

std::vector<LevelLoader::TileInfo> LevelLoader::decodeTiles(const json& layerData, int gridSize) {
    std::vector<TileInfo> result;

    // Si le layer contient des tiles au format grid
    if (layerData.contains("gridTiles")) {
        for (const auto& tile : layerData["gridTiles"]) {
            TileInfo info;
            info.tileId = tile["t"];
            info.x = tile["px"][0];
            info.y = tile["px"][1];

            // Vérifier s'il y a des flags de flip/rotation
            info.flipped = false;
            info.rotated = false;
            if (tile.contains("f")) {
                info.flipped = tile["f"] > 0;
            }
            if (tile.contains("r")) {
                info.rotated = tile["r"] > 0;
            }

            result.push_back(info);
        }
    }
    // Si le layer contient des auto-layers
    else if (layerData.contains("autoLayerTiles")) {
        for (const auto& tile : layerData["autoLayerTiles"]) {
            TileInfo info;
            info.tileId = tile["t"];
            info.x = tile["px"][0];
            info.y = tile["px"][1];
            info.flipped = false;
            info.rotated = false;

            result.push_back(info);
        }
    }

    return result;
}

json LevelLoader::getCurrentLevel(const json& project) {
    // Pour cet exemple, nous prenons le premier niveau trouvé
    if (project.contains("levels") && !project["levels"].empty()) {
        return project["levels"][0];
    }

    // S'il n'y a pas de niveaux dans le tableau "levels", chercher dans le monde
    if (project.contains("worlds") && !project["worlds"].empty()) {
        const auto& worlds = project["worlds"];
        for (const auto& world : worlds) {
            if (world.contains("levels") && !world["levels"].empty()) {
                return world["levels"][0];
            }
        }
    }

    return json();
}