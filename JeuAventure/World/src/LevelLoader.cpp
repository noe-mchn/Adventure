//#include "LevelLoader.h"
//#include "Level.h"
//#include "Tilemap.h"
//#include "RessourceManager.h"
//#include "Player.h"
//#include "Enemy.h"
//#include "Objects.h"
//#include <fstream>
//#include <iostream>
//#include <sstream>
//#include <nlohmann/json.hpp>
//
//using json = nlohmann::json;
//
//bool LevelLoader::loadFromFile(const std::string& filename, Level* level) {
//    if (!level) {
//        std::cerr << "Error: Level pointer is null" << std::endl;
//        return false;
//    }
//
//    std::ifstream file(filename);
//    if (!file.is_open()) {
//        std::cerr << "Error: Could not open file: " << filename << std::endl;
//        return false;
//    }
//
//    try {
//        json jsonData;
//        file >> jsonData;
//        file.close();
//
//        LevelData levelData = parseJson(jsonData);
//        applyLevelData(level, levelData);
//
//        return true;
//    }
//    catch (const std::exception& e) {
//        std::cerr << "Error parsing JSON file: " << e.what() << std::endl;
//        return false;
//    }
//}
//
//bool LevelLoader::loadFromJson(const std::string& jsonString, Level* level) {
//    if (!level) {
//        std::cerr << "Error: Level pointer is null" << std::endl;
//        return false;
//    }
//
//    try {
//        json jsonData = json::parse(jsonString);
//        LevelData levelData = parseJson(jsonData);
//        applyLevelData(level, levelData);
//
//        return true;
//    }
//    catch (const std::exception& e) {
//        std::cerr << "Error parsing JSON string: " << e.what() << std::endl;
//        return false;
//    }
//}
//
//LevelData LevelLoader::parseJson(const json& jsonData) {
//    LevelData levelData;
//
//    levelData.width = jsonData.value("width", 100);
//    levelData.height = jsonData.value("height", 100);
//    levelData.tileWidth = jsonData.value("tilewidth", 32);
//    levelData.tileHeight = jsonData.value("tileheight", 32);
//
//    levelData.backgroundPath = jsonData.value("backgroundPath", "");
//    levelData.musicPath = jsonData.value("musicPath", "");
//
//    if (jsonData.contains("properties") && jsonData["properties"].is_array()) {
//        levelData.properties = parseProperties(jsonData["properties"]);
//    }
//
//    if (jsonData.contains("layers") && jsonData["layers"].is_array()) {
//        for (const auto& layerJson : jsonData["layers"]) {
//            LayerData layer;
//            layer.name = layerJson.value("name", "");
//            layer.visible = layerJson.value("visible", true);
//
//            if (layerJson.contains("tiles") && layerJson["tiles"].is_array()) {
//                layer.tiles = parseTiles(layerJson["tiles"]);
//            }
//
//            if (layerJson.contains("entities") && layerJson["entities"].is_array()) {
//                layer.entities = parseEntities(layerJson["entities"]);
//            }
//
//            levelData.layers.push_back(layer);
//        }
//    }
//
//    return levelData;
//}
//
//std::vector<TileData> LevelLoader::parseTiles(const json& tilesJson) {
//    std::vector<TileData> tiles;
//
//    for (const auto& tileJson : tilesJson) {
//        TileData tile;
//        tile.id = tileJson.value("id", 0);
//        tile.x = tileJson.value("x", 0);
//        tile.y = tileJson.value("y", 0);
//        tile.collidable = tileJson.value("collidable", false);
//        tile.type = tileJson.value("type", "");
//
//        if (tileJson.contains("properties") && tileJson["properties"].is_array()) {
//            for (const auto& prop : tileJson["properties"]) {
//                if (prop.contains("name") && prop.contains("value")) {
//                    tile.properties.push_back(prop["value"].get<std::string>());
//                }
//            }
//        }
//
//        tiles.push_back(tile);
//    }
//
//    return tiles;
//}
//
//std::vector<EntityData> LevelLoader::parseEntities(const json& entitiesJson) {
//    std::vector<EntityData> entities;
//
//    for (const auto& entityJson : entitiesJson) {
//        EntityData entity;
//        entity.type = entityJson.value("type", "");
//        entity.name = entityJson.value("name", "");
//        entity.x = entityJson.value("x", 0.0f);
//        entity.y = entityJson.value("y", 0.0f);
//
//        if (entityJson.contains("properties") && entityJson["properties"].is_array()) {
//            entity.properties = parseProperties(entityJson["properties"]);
//        }
//
//        entities.push_back(entity);
//    }
//
//    return entities;
//}
//
//std::vector<std::pair<std::string, std::string>> LevelLoader::parseProperties(const json& propertiesJson) {
//    std::vector<std::pair<std::string, std::string>> properties;
//
//    for (const auto& propJson : propertiesJson) {
//        if (propJson.contains("name") && propJson.contains("value")) {
//            std::string name = propJson["name"];
//            std::string value;
//
//            if (propJson["value"].is_string()) {
//                value = propJson["value"].get<std::string>();
//            }
//            else if (propJson["value"].is_number()) {
//                value = std::to_string(propJson["value"].get<float>());
//            }
//            else if (propJson["value"].is_boolean()) {
//                value = propJson["value"].get<bool>() ? "true" : "false";
//            }
//            else {
//                value = propJson["value"].dump();
//            }
//
//            properties.emplace_back(name, value);
//        }
//    }
//
//    return properties;
//}
//
//void LevelLoader::applyLevelData(Level* level, const LevelData& levelData) {
//    level->setSize(levelData.width * levelData.tileWidth, levelData.height * levelData.tileHeight);
//    level->setTileSize(levelData.tileWidth, levelData.tileHeight);
//
//    if (!levelData.backgroundPath.empty()) {
//        level->setBackground(levelData.backgroundPath);
//    }
//
//    if (!levelData.musicPath.empty()) {
//        level->setMusic(levelData.musicPath);
//    }
//
//    for (const auto& [key, value] : levelData.properties) {
//        level->setProperty(key, value);
//    }
//
//    Tilemap* tilemap = level->getTilemap();
//    if (!tilemap) {
//        tilemap = new Tilemap(levelData.width, levelData.height);
//        level->setTilemap(tilemap);
//    }
//
//    tilemap->setTileSize(levelData.tileWidth, levelData.tileHeight);
//
//    for (const auto& layerData : levelData.layers) {
//        if (!layerData.visible) continue;
//
//        for (const auto& tileData : layerData.tiles) {
//            tilemap->setTile(tileData.x, tileData.y, tileData.id);
//
//            if (tileData.collidable) {
//                tilemap->setTileCollision(tileData.x, tileData.y, true);
//            }
//
//            if (!tileData.type.empty()) {
//                if (tileData.type == "spike") {
//                    Hazard* hazard = new Hazard(10);
//                    hazard->setPosition(tileData.x * levelData.tileWidth + levelData.tileWidth / 2,
//                        tileData.y * levelData.tileHeight + levelData.tileHeight / 2);
//                    hazard->setSize(levelData.tileWidth, levelData.tileHeight);
//                    level->addEntity(hazard);
//                }
//            }
//        }
//
//        for (const auto& entityData : layerData.entities) {
//            Entity* entity = nullptr;
//
//            if (entityData.type == "player") {
//                level->setPlayerStart(sf::Vector2f(entityData.x, entityData.y));
//            }
//            else if (entityData.type == "enemy") {
//                std::string enemyType = "basic";
//
//                for (const auto& [key, value] : entityData.properties) {
//                    if (key == "enemyType") {
//                        enemyType = value;
//                    }
//                }
//
//                Enemy* enemy = nullptr;
//
//                if (enemyType == "flying") {
//                    enemy = new Enemy(EnemyType::Flying);
//                }
//                else if (enemyType == "charging") {
//                    enemy = new Enemy(EnemyType::Charging);
//                }
//                else if (enemyType == "ranged") {
//                    enemy = new Enemy(EnemyType::Ranged);
//                }
//                else if (enemyType == "boss") {
//                    enemy = new Enemy(EnemyType::Boss);
//                }
//                else {
//                    enemy = new Enemy(EnemyType::Basic);
//                }
//
//                enemy->setPosition(entityData.x, entityData.y);
//                entity = enemy;
//            }
//            else if (entityData.type == "pickup") {
//                std::string pickupType = "coin";
//                int value = 1;
//
//                for (const auto& [key, value_str] : entityData.properties) {
//                    if (key == "pickupType") {
//                        pickupType = value_str;
//                    }
//                    else if (key == "value") {
//                        value = std::stoi(value_str);
//                    }
//                }
//
//                Pickup* pickup = new Pickup(pickupType, value);
//                pickup->setPosition(entityData.x, entityData.y);
//                entity = pickup;
//            }
//            else if (entityData.type == "platform") {
//                bool isMoving = false;
//                bool isFalling = false;
//
//                for (const auto& [key, value] : entityData.properties) {
//                    if (key == "moving" && value == "true") {
//                        isMoving = true;
//                    }
//                    else if (key == "falling" && value == "true") {
//                        isFalling = true;
//                    }
//                }
//
//                Platform* platform = new Platform(isMoving, isFalling);
//                platform->setPosition(entityData.x, entityData.y);
//
//                if (isMoving) {
//                    for (const auto& [key, value] : entityData.properties) {
//                        if (key.rfind("waypoint", 0) == 0) {
//                            std::istringstream ss(value);
//                            std::string xStr, yStr;
//                            std::getline(ss, xStr, ',');
//                            std::getline(ss, yStr);
//
//                            try {
//                                float x = std::stof(xStr);
//                                float y = std::stof(yStr);
//                                platform->addWaypoint(sf::Vector2f(x, y));
//                            }
//                            catch (const std::exception& e) {
//                                std::cerr << "Invalid waypoint format: " << value << std::endl;
//                            }
//                        }
//                    }
//                }
//
//                entity = platform;
//            }
//            else if (entityData.type == "trigger") {
//                Trigger* trigger = new Trigger();
//                trigger->setPosition(entityData.x, entityData.y);
//
//                float width = 32.0f;
//                float height = 32.0f;
//
//                std::string triggerTag = "";
//                bool triggerOnce = true;
//
//                for (const auto& [key, value] : entityData.properties) {
//                    if (key == "width") {
//                        width = std::stof(value);
//                    }
//                    else if (key == "height") {
//                        height = std::stof(value);
//                    }
//                    else if (key == "tag") {
//                        triggerTag = value;
//                    }
//                    else if (key == "triggerOnce") {
//                        triggerOnce = (value == "true");
//                    }
//                }
//
//                trigger->setSize(width, height);
//                trigger->setTriggerTag(triggerTag);
//                trigger->setTriggerOnce(triggerOnce);
//
//                entity = trigger;
//            }
//
//            if (entity) {
//                if (!entityData.name.empty()) {
//                    entity->setName(entityData.name);
//                }
//
//                level->addEntity(entity);
//            }
//        }
//    }
//
//    level->initialize();
//}
