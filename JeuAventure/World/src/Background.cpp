#include "Background.h"
#include "RessourceManager.h"
#include <iostream>

Background::Background()
    : m_cameraPosition(0.0f, 0.0f),
    m_size(2560, 1600),
    m_backgroundColor(sf::Color(40, 40, 60)),
    m_parallaxFactor(1.0f) {
}

Background::Background(const sf::Texture& texture)
    : m_cameraPosition(0.0f, 0.0f),
    m_backgroundColor(sf::Color(40, 40, 60)),
    m_parallaxFactor(1.0f) {
    addLayer(texture, 0.0f, true);
    m_size = sf::Vector2f(texture.getSize().x, texture.getSize().y);
}

Background::~Background() {
}

void Background::addLayer(const sf::Texture& texture, float scrollSpeed, bool repeat) {
    ParallaxLayer layer;
    layer.sprite.setTexture(texture);
    layer.repeat = repeat;

    if (repeat) {
        const_cast<sf::Texture&>(texture).setRepeated(true);
        layer.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(m_size.x * 2), static_cast<int>(m_size.y)));
    }

    m_layers.push_back(layer);
}

void Background::addLayer(const std::string& texturePath, float scrollSpeed, bool repeat) {
    sf::Texture* texture = nullptr;
    texture = RessourceManager::getInstance()->getTexture(texturePath);

    if (!texture) {
        if (RessourceManager::getInstance()->loadTexture(texturePath, texturePath)) {
            texture = RessourceManager::getInstance()->getTexture(texturePath);
        }
        else {
            std::cerr << "Failed to load background texture: " << texturePath << std::endl;
            return;
        }
    }

    if (texture) {
        addLayer(*texture, scrollSpeed, repeat);
    }
}

void Background::clearLayers() {
    m_layers.clear();
    m_textures.clear();
}

void Background::setBackgroundColor(const sf::Color& color) {
    m_backgroundColor = color;
}

void Background::setParallaxFactor(float factor) {
    m_parallaxFactor = factor;
}

void Background::setSize(const sf::Vector2f& size) {
    m_size = size;

    for (auto& layer : m_layers) {
        if (layer.repeat) {
            layer.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(m_size.x * 2), static_cast<int>(m_size.y)));
        }
    }
}

void Background::setScale(const sf::Vector2f& scale) {
    for (auto& layer : m_layers) {
        layer.sprite.setScale(scale);
    }
}


void Background::update(float dt) {
}

void Background::render(sf::RenderWindow& window) {
    sf::RectangleShape background(sf::Vector2f(window.getView().getSize().x, window.getView().getSize().y));
    background.setFillColor(m_backgroundColor);

    sf::View originalView = window.getView();
    window.setView(window.getDefaultView());
    window.draw(background);
    window.setView(originalView);

}


void Background::setCameraPosition(const sf::Vector2f& position) {
    m_cameraPosition = position;
}
