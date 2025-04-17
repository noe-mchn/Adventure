#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "Checkpoint.h"

class Entity;
class Tilemap;
class Background;
class Player;
class Checkpoint;

class Level {
private:
    float m_width;
    float m_height;

    int m_tileWidth;
    int m_tileHeight;

    std::unordered_map<std::string, std::string> m_properties;

    std::unique_ptr<Tilemap> m_tilemap;

    std::unique_ptr<Background> m_background;

    std::vector<std::unique_ptr<sf::Sprite>> m_layers;
    sf::Vector2f m_scale;

    std::vector<Entity*> m_entities;

    sf::Vector2f m_playerStartPosition;

    Player* m_player;

    std::vector<Checkpoint*> m_checkpoints;
    Checkpoint* m_activeCheckpoint;

    std::string m_musicPath;
    bool m_musicPlaying;

    std::string m_name;

    bool m_isCompleted;
    bool m_isLoaded;

    sf::FloatRect m_cameraBounds;

    float m_levelTimer;

public:
    Level(const std::string& name = "");
    ~Level();

    bool loadFromFile(const std::string& filename);
    void initialize();
    void reset();

    void update(float dt);
    void render(sf::RenderWindow& window);

    void addEntity(Entity* entity);
    void removeEntity(Entity* entity);
    Entity* findEntityByName(const std::string& name);
    void clearEntities();

    void setTilemap(Tilemap* tilemap);
    Tilemap* getTilemap() const;

    void setBackground(const std::string& texturePath);
    Background* getBackground() const;

    void setSize(float width, float height);
    float getWidth() const;
    float getHeight() const;

    void setTileSize(int width, int height);
    int getTileWidth() const;
    int getTileHeight() const;

    void setPlayerStart(const sf::Vector2f& position);
    sf::Vector2f getPlayerStart() const;

    void setPlayer(Player* player);
    Player* getPlayer() const;

    void addCheckpoint(Checkpoint* checkpoint);
    bool activateCheckpoint(Checkpoint* checkpoint);
    Checkpoint* getActiveCheckpoint() const;

    void setMusic(const std::string& musicPath);
    void playMusic();
    void stopMusic();

    void setProperty(const std::string& key, const std::string& value);
    std::string getProperty(const std::string& key) const;
    bool hasProperty(const std::string& key) const;

    void setCompleted(bool completed);
    bool isCompleted() const;

    const std::string& getName() const;
    sf::FloatRect getBounds() const;
    float getLevelTimer() const;

    void setCameraBounds(const sf::FloatRect& bounds);
    sf::FloatRect getCameraBounds() const;

    bool checkCollision(const sf::FloatRect& rect) const;
    std::vector<Entity*> getEntitiesInArea(const sf::FloatRect& area);

    void handleEvent(const sf::Event& event);

    void addPlatformLayer(sf::Texture* texture);

    void addTileLayer(std::unique_ptr<sf::Sprite> layer);

    // Ajout d'une couche de parallaxe
    void addParallaxLayer(sf::Sprite* sprite, const sf::Vector2f& parallaxFactor);

    // Définir une texture comme fond (alternative à setBackground)
    void setBackground(sf::Texture* texture);
};