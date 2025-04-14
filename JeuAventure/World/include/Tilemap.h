#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>

struct TileInfo {
    int id;
    bool collidable;
    std::string type;
    std::vector<std::string> properties;
};

class Tilemap {
private:
    int m_width;
    int m_height;
    int m_tileWidth;
    int m_tileHeight;

    std::vector<std::vector<TileInfo>> m_tiles;

    sf::Texture* m_tilesetTexture;

    int m_tilesetColumns;
    int m_tilesetRows;

    sf::VertexArray m_vertices;

    std::vector<sf::FloatRect> m_collisionBoxes;

    sf::FloatRect m_lastViewRect;
    std::vector<sf::Vector2i> m_visibleTiles;

    void rebuildVertexArray();

    void updateVisibleTiles(const sf::FloatRect& viewRect);

    sf::Vector2i getTilesetCoords(int id) const;

public:
    Tilemap(int width = 100, int height = 100);
    ~Tilemap();

    bool loadTileset(const std::string& texturePath, int tileWidth, int tileHeight);

    void setTile(int x, int y, int id);
    int getTile(int x, int y) const;

    void setTileCollision(int x, int y, bool collidable);
    bool isTileCollidable(int x, int y) const;

    void setTileType(int x, int y, const std::string& type);
    std::string getTileType(int x, int y) const;

    void resize(int width, int height);
    int getWidth() const;
    int getHeight() const;

    void setTileSize(int width, int height);
    int getTileWidth() const;
    int getTileHeight() const;

    sf::Vector2f tileToWorld(int x, int y) const;
    sf::Vector2i worldToTile(float x, float y) const;

    bool checkCollision(const sf::FloatRect& rect) const;
    std::vector<sf::FloatRect> getCollisionsInArea(const sf::FloatRect& area) const;

    void update(float dt);
    void render(sf::RenderWindow& window);

    void clear();
};
