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
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool LevelLoader::loadLevel(const std::string& jsonFilePath, Level* level) {
    if (!level) {
        std::cerr << "Error: Level pointer is null" << std::endl;
        return false;
    }

    try {
        RessourceManager* resourceManager = RessourceManager::getInstance();
        std::filesystem::path fullJsonPath = resourceManager->getResourcePath(jsonFilePath + "\\allData.json");

        std::cout << "Loading LDtk level from: " << fullJsonPath << std::endl;

        std::ifstream file(fullJsonPath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open LDtk file: " << fullJsonPath << std::endl;
            return false;
        }

        json projectData;
        file >> projectData;
        file.close();

        int defaultGridSize = projectData["defaultGridSize"];
        int defaultLevelWidth = projectData["defaultLevelWidth"];
        int defaultLevelHeight = projectData["defaultLevelHeight"];

        defaultLevelWidth = 2048;
        defaultLevelHeight = 512;

        std::cout << "Adjusted level dimensions: " << defaultLevelWidth << "x" << defaultLevelHeight
            << ", Grid size: " << defaultGridSize << std::endl;

        level->setSize(defaultLevelWidth, defaultLevelHeight);
        level->setTileSize(defaultGridSize, defaultGridSize);

        Tilemap* tilemap = level->getTilemap();
        if (!tilemap) {
            tilemap = new Tilemap(defaultLevelWidth / defaultGridSize, defaultLevelHeight / defaultGridSize);
            level->setTilemap(tilemap);
        }
        tilemap->setTileSize(defaultGridSize, defaultGridSize);

        if (resourceManager->loadTexture("background", "Background.png")) {
            sf::Texture* platformTexture = resourceManager->getTexture("background");
            if (platformTexture) {
                level->addPlatformLayer(platformTexture);
                std::cout << "Added basic platform layer" << std::endl;
            }
        }
        if (resourceManager->loadTexture("platform_base", "platform_base.png")) {
            sf::Texture* platformTexture = resourceManager->getTexture("platform_base");
            if (platformTexture) {
                level->addPlatformLayer(platformTexture);
                std::cout << "Added basic platform layer" << std::endl;
            }
        }
        else {
            sf::Image defaultImage;
            defaultImage.create(defaultLevelWidth, defaultLevelHeight, sf::Color(200, 200, 200));

            for (int y = 0; y < defaultLevelHeight; y += defaultGridSize * 5) {
                for (int x = 0; x < defaultLevelWidth; x++) {
                    defaultImage.setPixel(x, y, sf::Color::Black);
                }
            }

            std::unique_ptr<sf::Texture> defaultTexture = std::make_unique<sf::Texture>();
            if (defaultTexture->loadFromImage(defaultImage)) {
                sf::Texture* tex = resourceManager->getTexture("default_platform");
                if (tex) {
                    level->addPlatformLayer(tex);
                    std::cout << "Added default platform layer" << std::endl;
                }
            }
        }

        if (projectData.contains("levels") && !projectData["levels"].empty()) {
            const auto& firstLevel = projectData["levels"][0];
            if (firstLevel.contains("layerInstances")) {
                for (const auto& layer : firstLevel["layerInstances"]) {
                    if (layer["__type"] == "IntGrid") {
                        std::cout << "Processing collision layer: " << layer["__identifier"] << std::endl;
                        loadCollisionsFromIntGrid(level, layer, defaultGridSize);
                    }
                }
            }
        }

        sf::Vector2f playerStart(100, 100);
        level->setPlayerStart(playerStart);

        createDefaultEntities(level);

        level->initialize();

        std::cout << "Basic level loaded successfully" << std::endl;
        std::cout << "Actual rendering dimensions: " << level->getWidth() << "x" << level->getHeight() << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading level: " << e.what() << std::endl;
        return false;
    }
}

void LevelLoader::loadCollisionsFromIntGrid(Level* level, const json& layerData, int gridSize) {
    Tilemap* tilemap = level->getTilemap();
    if (!tilemap) return;

    int gridWidth = layerData["__cWid"];
    int gridHeight = layerData["__cHei"];

    if (layerData.contains("intGridCsv")) {
        const auto& intGridValues = layerData["intGridCsv"];

        int collisionCount = 0;

        for (int y = 0; y < gridHeight; y++) {
            for (int x = 0; x < gridWidth; x++) {
                int idx = y * gridWidth + x;
                if (idx < intGridValues.size()) {
                    int value = intGridValues[idx];
                    if (value > 0) { 
                        tilemap->setTile(x, y, value);
                        tilemap->setTileCollision(x, y, true);
                        collisionCount++;
                    }
                }
            }
        }

        std::cout << "Added " << collisionCount << " collision tiles from layer " << (std::string)layerData["__identifier"] << std::endl;
    }
}

void LevelLoader::loadTileLayer(Level* level, const json& layerData, const json& project, const std::string& basePath) {
    int tilesetDefUid = layerData["__tilesetDefUid"];

    json tileset = findTileset(project, tilesetDefUid);
    if (tileset.empty()) {
        std::cerr << "Tileset not found for UID: " << tilesetDefUid << std::endl;
        return;
    }

    std::string relPath = tileset["relPath"];
    std::string tilesetId = tileset["identifier"];

    std::filesystem::path relPathObj(relPath);
    std::string fileName = relPathObj.filename().string();

    sf::Texture* texture = nullptr;
    RessourceManager* resourceManager = RessourceManager::getInstance();

    if (resourceManager->loadTexture(tilesetId, fileName)) {
        texture = resourceManager->getTexture(tilesetId);
    }
    else if (resourceManager->loadTexture(tilesetId, relPath)) {
        texture = resourceManager->getTexture(tilesetId);
    }
    else if (std::filesystem::exists(relPath)) {
        if (resourceManager->loadTexture(tilesetId, relPath)) {
            texture = resourceManager->getTexture(tilesetId);
        }
    }
    else {
        std::vector<std::string> searchPaths = {
            "Textures/" + fileName,
            "tilesets/" + fileName,
            fileName
        };

        for (const auto& path : searchPaths) {
            if (resourceManager->loadTexture(tilesetId, path)) {
                texture = resourceManager->getTexture(tilesetId);
                std::cout << "Loaded tileset texture from: " << path << std::endl;
                break;
            }
        }
    }

    if (!texture) {
        std::cerr << "Failed to load tileset texture: " << relPath << std::endl;
        return;
    }

    std::cout << "Successfully loaded tileset: " << tilesetId << ", texture size: "
        << texture->getSize().x << "x" << texture->getSize().y << std::endl;

    std::unique_ptr<sf::Sprite> layerSprite = std::make_unique<sf::Sprite>();

    sf::RenderTexture renderTexture;
    if (!renderTexture.create(level->getWidth(), level->getHeight())) {
        std::cerr << "Failed to create render texture for layer" << std::endl;
        return;
    }
    renderTexture.clear(sf::Color::Transparent);

    int tileSize = tileset["tileGridSize"];
    int tilesetColumns = tileset["__cWid"];

    std::cout << "Tileset grid size: " << tileSize << ", columns: " << tilesetColumns << std::endl;

    if (layerData.contains("gridTiles")) {
        sf::Sprite tileSprite(*texture);

        for (const auto& tile : layerData["gridTiles"]) {
            int tileId = tile["t"];

            int destX = tile["px"][0];
            int destY = tile["px"][1];

            int srcX = (tileId % tilesetColumns) * tileSize;
            int srcY = (tileId / tilesetColumns) * tileSize;

            tileSprite.setTextureRect(sf::IntRect(srcX, srcY, tileSize, tileSize));
            tileSprite.setPosition(destX, destY);

            bool flipX = false;
            bool flipY = false;
            if (tile.contains("f")) flipX = (tile["f"] == 1 || tile["f"] == 3);
            if (tile.contains("f")) flipY = (tile["f"] == 2 || tile["f"] == 3);

            sf::Vector2f scale(1.0f, 1.0f);
            if (flipX) scale.x = -1.0f;
            if (flipY) scale.y = -1.0f;

            tileSprite.setScale(scale);

            if (flipX || flipY) {
                tileSprite.setOrigin(
                    flipX ? tileSize : 0,
                    flipY ? tileSize : 0
                );
            }
            else {
                tileSprite.setOrigin(0, 0);
            }

            renderTexture.draw(tileSprite);

            tileSprite.setScale(1.0f, 1.0f);
            tileSprite.setOrigin(0, 0);
        }
    }

    renderTexture.display();

    layerSprite->setTexture(renderTexture.getTexture());

    std::string layerId = layerData["__identifier"];

    if (layerId == "RealBackground" || layerId == "Back") {
        sf::Texture bgTexture = renderTexture.getTexture();
        level->setBackground(texture);
        std::cout << "Set layer as background: " << layerId << std::endl;
    }
    else {
        level->addTileLayer(std::move(layerSprite));
        std::cout << "Added layer as normal layer: " << layerId << std::endl;
    }
}

void LevelLoader::loadEntities(Level* level, const json& layerData) {
    if (!layerData.contains("entityInstances")) return;

    const auto& entities = layerData["entityInstances"];

    for (const auto& entityData : entities) {
        std::string entityType = entityData["__identifier"];

        Entity* entity = createEntityByType(entityType, entityData);

        if (entity) {
            int gridSize = entityData["__gridSize"].get<int>();
            int pixelX = entityData["__grid"][0].get<int>() * gridSize;
            int pixelY = entityData["__grid"][1].get<int>() * gridSize;
            entity->setPosition(pixelX, pixelY);

            level->addEntity(entity);

            std::cout << "Added entity: " << entityType << " at position " << pixelX << "," << pixelY << std::endl;
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

std::vector<LevelLoader::TileInfo> LevelLoader::decodeTiles(const json& layerData, int gridSize) {
    std::vector<TileInfo> result;

    if (layerData.contains("gridTiles")) {
        for (const auto& tile : layerData["gridTiles"]) {
            TileInfo info;
            info.tileId = tile["t"];

            info.x = tile["px"][0];
            info.y = tile["px"][1];

            info.flipped = false;
            info.rotated = false;
            if (tile.contains("f")) {
                int f = tile["f"];
                info.flipped = (f == 1 || f == 3);
                info.flipY = (f == 2 || f == 3);
            }

            result.push_back(info);
        }
    }
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
    if (project.contains("levels") && !project["levels"].empty()) {
        return project["levels"][0];
    }

    return json();
}

Entity* LevelLoader::createEntityByType(const std::string& type, const json& entityData) {
    Entity* entity = nullptr;

    if (type == "Player" || type == "PlayerStart") 
    {
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

    if (entity) {
        int width = entityData.value("width", 16);
        int height = entityData.value("height", 16);
        entity->setSize(sf::Vector2f(width, height));
    }

    return entity;
}

void LevelLoader::createDefaultEntities(Level* level) {
    for (int i = 0; i < 10; i++) {
        float x = 200 + i * 180;
        float y = 350;

        Pickup* bitcoin = new Pickup("Bitcoin", 1);
        bitcoin->setPosition(x, y);
        level->addEntity(bitcoin);
    }

    Pickup* health1 = new Pickup("health", 20);
    health1->setPosition(800, 300);
    level->addEntity(health1);

    Pickup* health2 = new Pickup("health", 20);
    health2->setPosition(1500, 300);
    level->addEntity(health2);

    Checkpoint* checkpoint1 = new Checkpoint();
    checkpoint1->setPosition(700, 400);
    level->addEntity(checkpoint1);

    Checkpoint* checkpoint2 = new Checkpoint();
    checkpoint2->setPosition(1400, 400);
    level->addEntity(checkpoint2);

    Trigger* finishTrigger = new Trigger();
    finishTrigger->setPosition(1900, 400);
    finishTrigger->setSize(32, 64);
    finishTrigger->setTriggerTag("finish");
    level->addEntity(finishTrigger);

    std::cout << "Created default entities for testing" << std::endl;
}
