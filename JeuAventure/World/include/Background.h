#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Render.h"

class Background {
private:
    std::vector<ParallaxLayer> m_layers;

    sf::Sprite m_sprite;

    std::vector<std::unique_ptr<sf::Texture>> m_textures;

    sf::Vector2f m_cameraPosition;

    sf::Vector2f m_size;

    sf::Color m_backgroundColor;

    float m_parallaxFactor;

public:
    Background();
    explicit Background(const sf::Texture& texture);
    ~Background();

    void addLayer(const sf::Texture& texture, float scrollSpeed = 0.5f, bool repeat = true);
    void addLayer(const std::string& texturePath, float scrollSpeed = 0.5f, bool repeat = true);
    void clearLayers();

    void setBackgroundColor(const sf::Color& color);
    void setParallaxFactor(float factor);
    void setSize(const sf::Vector2f& size);

    void update(float dt);
    void render(sf::RenderWindow& window);

    void setScale(const sf::Vector2f& scale);

    void setCameraPosition(const sf::Vector2f& position);
};
