#include "Tilemap.h"
#include "RessourceManager.h"
#include <iostream>
#include <algorithm>

Tilemap::Tilemap(int width, int height)
    : m_width(width),
    m_height(height),
    m_tileWidth(32),
    m_tileHeight(32),
    m_tilesetTexture(nullptr),
    m_tilesetColumns(0),
    m_tilesetRows(0),
    m_vertices(sf::Quads) {
    m_tiles.resize(m_height, std::vector<TileInfo>(m_width));
    m_vertices.resize(m_width * m_height * 4);

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            m_tiles[y][x].id = -1;
            m_tiles[y][x].collidable = false;
        }
    }

    m_lastViewRect = sf::FloatRect(0.0f, 0.0f, 800.0f, 600.0f);
}

Tilemap::~Tilemap() {}

bool Tilemap::loadTileset(const std::string& texturePath, int tileWidth, int tileHeight) {
    if (!RessourceManager::getInstance()->loadTexture("tileset", texturePath)) {
        std::cerr << "Failed to load tileset texture: " << texturePath << std::endl;
        return false;
    }

    m_tilesetTexture = RessourceManager::getInstance()->getTexture("tileset");
    m_tileWidth = tileWidth;
    m_tileHeight = tileHeight;

    if (m_tilesetTexture) {
        m_tilesetColumns = m_tilesetTexture->getSize().x / m_tileWidth;
        m_tilesetRows = m_tilesetTexture->getSize().y / m_tileHeight;
        rebuildVertexArray();
        return true;
    }

    return false;
}

void Tilemap::setTile(int x, int y, int id) {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        m_tiles[y][x].id = id;
        int index = (y * m_width + x) * 4;

        if (index + 3 < m_vertices.getVertexCount()) {
            m_vertices[index].position = sf::Vector2f(x * m_tileWidth, y * m_tileHeight);
            m_vertices[index + 1].position = sf::Vector2f((x + 1) * m_tileWidth, y * m_tileHeight);
            m_vertices[index + 2].position = sf::Vector2f((x + 1) * m_tileWidth, (y + 1) * m_tileHeight);
            m_vertices[index + 3].position = sf::Vector2f(x * m_tileWidth, (y + 1) * m_tileHeight);

            if (id >= 0 && m_tilesetTexture) {
                sf::Vector2i texCoords = getTilesetCoords(id);

                m_vertices[index].texCoords = sf::Vector2f(texCoords.x, texCoords.y);
                m_vertices[index + 1].texCoords = sf::Vector2f(texCoords.x + m_tileWidth, texCoords.y);
                m_vertices[index + 2].texCoords = sf::Vector2f(texCoords.x + m_tileWidth, texCoords.y + m_tileHeight);
                m_vertices[index + 3].texCoords = sf::Vector2f(texCoords.x, texCoords.y + m_tileHeight);
            }
            else {
                m_vertices[index].texCoords = sf::Vector2f(0, 0);
                m_vertices[index + 1].texCoords = sf::Vector2f(0, 0);
                m_vertices[index + 2].texCoords = sf::Vector2f(0, 0);
                m_vertices[index + 3].texCoords = sf::Vector2f(0, 0);
            }
        }

        if (m_tiles[y][x].collidable) {
            sf::FloatRect collisionBox(x * m_tileWidth, y * m_tileHeight, m_tileWidth, m_tileHeight);
            auto it = std::find(m_collisionBoxes.begin(), m_collisionBoxes.end(), collisionBox);
            if (it == m_collisionBoxes.end()) {
                m_collisionBoxes.push_back(collisionBox);
            }
        }
        else {
            sf::FloatRect collisionBox(x * m_tileWidth, y * m_tileHeight, m_tileWidth, m_tileHeight);
            auto it = std::find(m_collisionBoxes.begin(), m_collisionBoxes.end(), collisionBox);
            if (it != m_collisionBoxes.end()) {
                m_collisionBoxes.erase(it);
            }
        }
    }
}

int Tilemap::getTile(int x, int y) const {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        return m_tiles[y][x].id;
    }
    return -1;
}

void Tilemap::setTileCollision(int x, int y, bool collidable) {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        m_tiles[y][x].collidable = collidable;
        sf::FloatRect collisionBox(x * m_tileWidth, y * m_tileHeight, m_tileWidth, m_tileHeight);

        if (collidable) {
            auto it = std::find(m_collisionBoxes.begin(), m_collisionBoxes.end(), collisionBox);
            if (it == m_collisionBoxes.end()) {
                m_collisionBoxes.push_back(collisionBox);
            }
        }
        else {
            auto it = std::find(m_collisionBoxes.begin(), m_collisionBoxes.end(), collisionBox);
            if (it != m_collisionBoxes.end()) {
                m_collisionBoxes.erase(it);
            }
        }
    }
}

bool Tilemap::isTileCollidable(int x, int y) const {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        return m_tiles[y][x].collidable;
    }
    return false;
}

void Tilemap::setTileType(int x, int y, const std::string& type) {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        m_tiles[y][x].type = type;
    }
}

std::string Tilemap::getTileType(int x, int y) const {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        return m_tiles[y][x].type;
    }
    return "";
}

void Tilemap::resize(int width, int height) {
    if (width <= 0 || height <= 0) return;

    auto oldTiles = m_tiles;
    int oldWidth = m_width;
    int oldHeight = m_height;

    m_width = width;
    m_height = height;
    m_tiles.resize(m_height, std::vector<TileInfo>(m_width));

    for (int y = 0; y < std::min(oldHeight, m_height); ++y) {
        for (int x = 0; x < std::min(oldWidth, m_width); ++x) {
            m_tiles[y][x] = oldTiles[y][x];
        }
    }

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            if (y >= oldHeight || x >= oldWidth) {
                m_tiles[y][x].id = -1;
                m_tiles[y][x].collidable = false;
            }
        }
    }

    m_vertices.resize(m_width * m_height * 4);
    rebuildVertexArray();

    m_collisionBoxes.clear();
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            if (m_tiles[y][x].collidable) {
                sf::FloatRect collisionBox(x * m_tileWidth, y * m_tileHeight, m_tileWidth, m_tileHeight);
                m_collisionBoxes.push_back(collisionBox);
            }
        }
    }
}

int Tilemap::getWidth() const {
    return m_width;
}

int Tilemap::getHeight() const {
    return m_height;
}

void Tilemap::setTileSize(int width, int height) {
    if (width <= 0 || height <= 0) return;

    m_tileWidth = width;
    m_tileHeight = height;

    if (m_tilesetTexture) {
        m_tilesetColumns = m_tilesetTexture->getSize().x / m_tileWidth;
        m_tilesetRows = m_tilesetTexture->getSize().y / m_tileHeight;
    }

    rebuildVertexArray();

    m_collisionBoxes.clear();
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            if (m_tiles[y][x].collidable) {
                sf::FloatRect collisionBox(x * m_tileWidth, y * m_tileHeight, m_tileWidth, m_tileHeight);
                m_collisionBoxes.push_back(collisionBox);
            }
        }
    }
}

int Tilemap::getTileWidth() const {
    return m_tileWidth;
}

int Tilemap::getTileHeight() const {
    return m_tileHeight;
}

sf::Vector2f Tilemap::tileToWorld(int x, int y) const {
    return sf::Vector2f(x * m_tileWidth, y * m_tileHeight);
}

sf::Vector2i Tilemap::worldToTile(float x, float y) const {
    return sf::Vector2i(static_cast<int>(x / m_tileWidth), static_cast<int>(y / m_tileHeight));
}

bool Tilemap::checkCollision(const sf::FloatRect& rect) const {
    sf::Vector2i topLeft = worldToTile(rect.left, rect.top);
    sf::Vector2i bottomRight = worldToTile(rect.left + rect.width, rect.top + rect.height);

    topLeft.x = std::max(0, topLeft.x);
    topLeft.y = std::max(0, topLeft.y);
    bottomRight.x = std::min(m_width - 1, bottomRight.x);
    bottomRight.y = std::min(m_height - 1, bottomRight.y);

    for (int y = topLeft.y; y <= bottomRight.y; ++y) {
        for (int x = topLeft.x; x <= bottomRight.x; ++x) {
            if (m_tiles[y][x].collidable) {
                sf::FloatRect tileRect(x * m_tileWidth, y * m_tileHeight, m_tileWidth, m_tileHeight);
                if (rect.intersects(tileRect)) {
                    return true;
                }
            }
        }
    }

    return false;
}

std::vector<sf::FloatRect> Tilemap::getCollisionsInArea(const sf::FloatRect& area) const {
    std::vector<sf::FloatRect> collisions;

    sf::Vector2i topLeft = worldToTile(area.left, area.top);
    sf::Vector2i bottomRight = worldToTile(area.left + area.width, area.top + area.height);

    topLeft.x = std::max(0, topLeft.x);
    topLeft.y = std::max(0, topLeft.y);
    bottomRight.x = std::min(m_width - 1, bottomRight.x);
    bottomRight.y = std::min(m_height - 1, bottomRight.y);

    for (int y = topLeft.y; y <= bottomRight.y; ++y) {
        for (int x = topLeft.x; x <= bottomRight.x; ++x) {
            if (m_tiles[y][x].collidable) {
                sf::FloatRect tileRect(x * m_tileWidth, y * m_tileHeight, m_tileWidth, m_tileHeight);
                if (area.intersects(tileRect)) {
                    collisions.push_back(tileRect);
                }
            }
        }
    }

    return collisions;
}

void Tilemap::update(float dt) {}

void Tilemap::render(sf::RenderWindow& window) {
    sf::FloatRect viewRect = window.getView().getViewport();
    viewRect.left = window.getView().getCenter().x - window.getView().getSize().x / 2.0f;
    viewRect.top = window.getView().getCenter().y - window.getView().getSize().y / 2.0f;
    viewRect.width = window.getView().getSize().x;
    viewRect.height = window.getView().getSize().y;

    if (viewRect != m_lastViewRect) {
        updateVisibleTiles(viewRect);
        m_lastViewRect = viewRect;
    }

    if (m_tilesetTexture) {
        sf::VertexArray visibleVertices(sf::Quads);

        for (const auto& tilePos : m_visibleTiles) {
            int x = tilePos.x;
            int y = tilePos.y;
            int tileId = m_tiles[y][x].id;

            if (tileId >= 0) {
                int index = (y * m_width + x) * 4;

                visibleVertices.append(m_vertices[index]);
                visibleVertices.append(m_vertices[index + 1]);
                visibleVertices.append(m_vertices[index + 2]);
                visibleVertices.append(m_vertices[index + 3]);
            }
        }

        sf::RenderStates states;
        states.texture = m_tilesetTexture;
        window.draw(visibleVertices, states);

        sf::RectangleShape collisionTile;
        collisionTile.setSize(sf::Vector2f(m_tileWidth, m_tileHeight));
        collisionTile.setFillColor(sf::Color(255, 0, 0, 100));

        for (int y = 0; y < m_height; y++) {
            for (int x = 0; x < m_width; x++) {
                if (isTileCollidable(x, y)) {
                    collisionTile.setPosition(x * m_tileWidth, y * m_tileHeight);
                    window.draw(collisionTile);
                }
            }
        }
    }
}

void Tilemap::clear() {
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            m_tiles[y][x].id = -1;
            m_tiles[y][x].collidable = false;
            m_tiles[y][x].type = "";
            m_tiles[y][x].properties.clear();
        }
    }

    m_collisionBoxes.clear();
    rebuildVertexArray();
}

void Tilemap::rebuildVertexArray() {
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            int tileId = m_tiles[y][x].id;
            int index = (y * m_width + x) * 4;

            m_vertices[index].position = sf::Vector2f(x * m_tileWidth, y * m_tileHeight);
            m_vertices[index + 1].position = sf::Vector2f((x + 1) * m_tileWidth, y * m_tileHeight);
            m_vertices[index + 2].position = sf::Vector2f((x + 1) * m_tileWidth, (y + 1) * m_tileHeight);
            m_vertices[index + 3].position = sf::Vector2f(x * m_tileWidth, (y + 1) * m_tileHeight);

            if (tileId >= 0 && m_tilesetTexture) {
                sf::Vector2i texCoords = getTilesetCoords(tileId);

                m_vertices[index].texCoords = sf::Vector2f(texCoords.x, texCoords.y);
                m_vertices[index + 1].texCoords = sf::Vector2f(texCoords.x + m_tileWidth, texCoords.y);
                m_vertices[index + 2].texCoords = sf::Vector2f(texCoords.x + m_tileWidth, texCoords.y + m_tileHeight);
                m_vertices[index + 3].texCoords = sf::Vector2f(texCoords.x, texCoords.y + m_tileHeight);
            }
            else {
                m_vertices[index].texCoords = sf::Vector2f(0, 0);
                m_vertices[index + 1].texCoords = sf::Vector2f(0, 0);
                m_vertices[index + 2].texCoords = sf::Vector2f(0, 0);
                m_vertices[index + 3].texCoords = sf::Vector2f(0, 0);
            }
        }
    }
}

void Tilemap::updateVisibleTiles(const sf::FloatRect& viewRect) {
    m_visibleTiles.clear();

    sf::Vector2i topLeft = worldToTile(viewRect.left, viewRect.top);
    sf::Vector2i bottomRight = worldToTile(viewRect.left + viewRect.width, viewRect.top + viewRect.height);

    topLeft.x = std::max(0, topLeft.x - 1);
    topLeft.y = std::max(0, topLeft.y - 1);
    bottomRight.x = std::min(m_width - 1, bottomRight.x + 1);
    bottomRight.y = std::min(m_height - 1, bottomRight.y + 1);

    for (int y = topLeft.y; y <= bottomRight.y; ++y) {
        for (int x = topLeft.x; x <= bottomRight.x; ++x) {
            if (m_tiles[y][x].id >= 0) {
                m_visibleTiles.emplace_back(x, y);
            }
        }
    }
}

sf::Vector2i Tilemap::getTilesetCoords(int id) const {
    if (id < 0 || !m_tilesetTexture || m_tilesetColumns <= 0) {
        return sf::Vector2i(0, 0);
    }

    int x = (id % m_tilesetColumns) * m_tileWidth;
    int y = (id / m_tilesetColumns) * m_tileHeight;

    return sf::Vector2i(x, y);
}

