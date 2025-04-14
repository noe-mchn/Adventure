#include "Level.h"
#include "Tilemap.h"
#include "Background.h"
#include "LevelLoader.h"
#include "Entity.h"
#include "Player.h"
#include "EventSystem.h"
#include "RessourceManager.h"
#include <iostream>
#include <algorithm>

Level::Level(const std::string& name)
    : m_width(0.0f),
    m_height(0.0f),
    m_tileWidth(32),
    m_tileHeight(32),
    m_player(nullptr),
    m_activeCheckpoint(nullptr),
    m_musicPlaying(false),
    m_name(name),
    m_isCompleted(false),
    m_isLoaded(false),
    m_levelTimer(0.0f) {
    m_tilemap = std::make_unique<Tilemap>(100, 100);
    m_cameraBounds = sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f);
    EventSystem::getInstance()->addEventListener("PlayerDied", [this](const std::map<std::string, std::any>& params) {});
}

Level::~Level() {
    clearEntities();
}

bool Level::loadFromFile(const std::string& filename) {
    bool success = LevelLoader::loadFromFile(filename, this);

    if (success) {
        m_isLoaded = true;
        EventSystem::getInstance()->triggerEvent("LevelLoaded", {
            {"level", this},
            {"name", m_name}
            });
    }

    return success;
}

void Level::initialize() {
    for (auto* entity : m_entities) {
        if (entity) {
            entity->setLevel(this);
            entity->initialize();
        }
    }

    if (!m_musicPath.empty()) {
        playMusic();
    }

    if (m_cameraBounds.width <= 0.0f || m_cameraBounds.height <= 0.0f) {
        m_cameraBounds = sf::FloatRect(0.0f, 0.0f, m_width, m_height);
    }
}

void Level::reset() {
    m_levelTimer = 0.0f;
    m_isCompleted = false;

    if (m_player) {
        m_player->setPosition(m_playerStartPosition);
    }

    for (auto* entity : m_entities) {
        if (entity && entity != m_player) {}
    }

    m_activeCheckpoint = nullptr;
}

void Level::update(float dt) {
    m_levelTimer += dt;

    auto it = m_entities.begin();
    while (it != m_entities.end()) {
        Entity* entity = *it;
        if (entity && entity->isActive()) {
            entity->update(dt);
            ++it;
        }
        else if (entity && !entity->isActive()) {
            it = m_entities.erase(it);
            delete entity;
        }
        else {
            it = m_entities.erase(it);
        }
    }

    if (m_tilemap) { m_tilemap->update(dt); }
    if (m_background) { m_background->update(dt); }

    for (Checkpoint* checkpoint : m_checkpoints) {
        if (checkpoint && checkpoint->isActive()) {
            checkpoint->update(dt);
        }
    }
}

void Level::render(sf::RenderWindow& window) {
    if (m_background) { m_background->render(window); }
    if (m_tilemap) { m_tilemap->render(window); }

    std::sort(m_entities.begin(), m_entities.end(),
        [](Entity* a, Entity* b) {
            if (a && b) {
                return a->getPosition().y < b->getPosition().y;
            }
            return a != nullptr;
        }
    );

    for (Entity* entity : m_entities) {
        if (entity && entity->isActive() && entity->isVisible()) {
            entity->render(window);
        }
    }

    for (Checkpoint* checkpoint : m_checkpoints) {
        if (checkpoint && checkpoint->isActive() && checkpoint->isVisible()) {
            checkpoint->render(window);
        }
    }
}


void Level::addEntity(Entity* entity) {
    if (entity) {
        m_entities.push_back(entity);
        entity->setLevel(this);
    }
}

void Level::removeEntity(Entity* entity) {
    if (!entity) return;

    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it != m_entities.end()) {
        m_entities.erase(it);
    }
}

Entity* Level::findEntityByName(const std::string& name) {
    for (auto* entity : m_entities) {
        if (entity && entity->getName() == name) {
            return entity;
        }
    }
    return nullptr;
}

void Level::clearEntities() {
    for (auto* entity : m_entities) {
        delete entity;
    }
    m_entities.clear();
    m_checkpoints.clear();
}


void Level::setTilemap(Tilemap* tilemap) {
    if (tilemap) {
        m_tilemap.reset(tilemap);
    }
}

Tilemap* Level::getTilemap() const {
    return m_tilemap.get();
}

void Level::setBackground(const std::string& texturePath) {
    sf::Texture* texture = RessourceManager::getInstance()->getTexture(texturePath);

    if (texture) {
        m_background = std::make_unique<Background>(*texture);
    }
    else {
        if (RessourceManager::getInstance()->loadTexture("background", texturePath)) {
            texture = RessourceManager::getInstance()->getTexture("background");
            m_background = std::make_unique<Background>(*texture);
        }
        else {
            std::cerr << "Failed to load background texture: " << texturePath << std::endl;
        }
    }
}

Background* Level::getBackground() const {
    return m_background.get();
}

void Level::setSize(float width, float height) {
    m_width = width;
    m_height = height;

    if (m_tilemap) {
        m_tilemap->resize(static_cast<int>(width / m_tileWidth) + 1,
            static_cast<int>(height / m_tileHeight) + 1);
    }
}

float Level::getWidth() const {
    return m_width;
}

float Level::getHeight() const {
    return m_height;
}

void Level::setTileSize(int width, int height) {
    m_tileWidth = width;
    m_tileHeight = height;

    if (m_tilemap) {
        m_tilemap->setTileSize(width, height);
    }
}

int Level::getTileWidth() const {
    return m_tileWidth;
}

int Level::getTileHeight() const {
    return m_tileHeight;
}

void Level::setPlayerStart(const sf::Vector2f& position) {
    m_playerStartPosition = position;

    if (m_player) {
        m_player->setPosition(position);
    }
}

sf::Vector2f Level::getPlayerStart() const {
    return m_playerStartPosition;
}

void Level::setPlayer(Player* player) {
    m_player = player;

    if (m_player) {
        if (std::find(m_entities.begin(), m_entities.end(), m_player) == m_entities.end()) {
            addEntity(m_player);
        }

        m_player->setPosition(m_playerStartPosition);
    }
}

Player* Level::getPlayer() const {
    return m_player;
}

void Level::addCheckpoint(Checkpoint* checkpoint) {
    if (checkpoint) {
        m_checkpoints.push_back(checkpoint);
    }
}

bool Level::activateCheckpoint(Checkpoint* checkpoint) {
    auto it = std::find(m_checkpoints.begin(), m_checkpoints.end(), checkpoint);
    if (it != m_checkpoints.end()) {
        m_activeCheckpoint = checkpoint;

        EventSystem::getInstance()->triggerEvent("CheckpointActivated", {
            {"checkpoint", checkpoint},
            {"level", this}
            });

        return true;
    }
    return false;
}

Checkpoint* Level::getActiveCheckpoint() const {
    return m_activeCheckpoint;
}

void Level::setMusic(const std::string& musicPath) {
    m_musicPath = musicPath;
}

void Level::playMusic() {
    if (!m_musicPath.empty() && !m_musicPlaying) {
        EventSystem::getInstance()->triggerEvent("PlayMusic", {
            {"path", m_musicPath},
            {"loop", true}
            });

        m_musicPlaying = true;
    }
}

void Level::stopMusic() {
    if (m_musicPlaying) {
        EventSystem::getInstance()->triggerEvent("StopMusic", {});
        m_musicPlaying = false;
    }
}

void Level::setProperty(const std::string& key, const std::string& value) {
    m_properties[key] = value;
}

std::string Level::getProperty(const std::string& key) const {
    auto it = m_properties.find(key);
    if (it != m_properties.end()) {
        return it->second;
    }
    return "";
}

bool Level::hasProperty(const std::string& key) const {
    return m_properties.find(key) != m_properties.end();
}

void Level::setCompleted(bool completed) {
    m_isCompleted = completed;

    if (m_isCompleted) {
        EventSystem::getInstance()->triggerEvent("LevelCompleted", {
            {"level", this},
            {"name", m_name},
            {"time", m_levelTimer}
            });
    }
}

bool Level::isCompleted() const {
    return m_isCompleted;
}

const std::string& Level::getName() const {
    return m_name;
}

sf::FloatRect Level::getBounds() const {
    return sf::FloatRect(0.0f, 0.0f, m_width, m_height);
}

float Level::getLevelTimer() const {
    return m_levelTimer;
}

void Level::setCameraBounds(const sf::FloatRect& bounds) {
    m_cameraBounds = bounds;
}

sf::FloatRect Level::getCameraBounds() const {
    return m_cameraBounds.width > 0.0f && m_cameraBounds.height > 0.0f ?
        m_cameraBounds :
        sf::FloatRect(0.0f, 0.0f, m_width, m_height);
}

bool Level::checkCollision(const sf::FloatRect& rect) const {
    if (m_tilemap) {
        return m_tilemap->checkCollision(rect);
    }
    return false;
}

std::vector<Entity*> Level::getEntitiesInArea(const sf::FloatRect& area) {
    std::vector<Entity*> result;

    for (auto* entity : m_entities) {
        if (entity && entity->isActive() && entity->getBounds().intersects(area)) {
            result.push_back(entity);
        }
    }

    return result;
}

void Level::handleEvent(const sf::Event& event) {
    for (auto* entity : m_entities) {
        if (entity && entity->isActive()) {
            entity->handleEvents(event);
        }
    }
}
