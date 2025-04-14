#include "Render.h"
#include "Entity.h"
#include <algorithm>
#include <iostream>
#include <sstream>

Render::Render() :
    m_window(nullptr),
    m_camera(nullptr),
    m_currentLevel(nullptr),
    m_currentShader(nullptr),
    m_usePostProcessing(false),
    m_fadeAlpha(0.0f),
    m_fadeColor(sf::Color::Black),
    m_vignetteIntensity(0.0f),
    m_parallaxOffset(0.0f, 0.0f)
{
}

Render::~Render() {
    clearRenderables();
    clearParallaxLayers();
    m_shaders.clear();
}

void Render::initialize(sf::RenderWindow* window, Camera* camera) {
    m_window = window;
    m_camera = camera;

    m_renderTexture.create(window->getSize().x, window->getSize().y);
}

void Render::setCurrentLevel(Level* level) {
    m_currentLevel = level;
}

Level* Render::getCurrentLevel() const {
    return m_currentLevel;
}

void Render::addEntity(Entity* entity, RenderLayer layer, int zOrder) {
    if (!entity) return;

    Renderable renderable;
    renderable.entity = entity;
    renderable.drawable = nullptr;
    renderable.layer = layer;
    renderable.zOrder = zOrder;
    renderable.visible = entity->isVisible();

    m_renderables.push_back(renderable);
}

void Render::addDrawable(sf::Drawable* drawable, RenderLayer layer, int zOrder) {
    if (!drawable) return;

    Renderable renderable;
    renderable.entity = nullptr;
    renderable.drawable = drawable;
    renderable.layer = layer;
    renderable.zOrder = zOrder;
    renderable.visible = true;

    m_renderables.push_back(renderable);
}

void Render::removeEntity(Entity* entity) {
    if (!entity) return;

    m_renderables.erase(
        std::remove_if(m_renderables.begin(), m_renderables.end(),
            [entity](const Renderable& r) { return r.entity == entity; }),
        m_renderables.end()
    );
}

void Render::removeDrawable(sf::Drawable* drawable) {
    if (!drawable) return;

    m_renderables.erase(
        std::remove_if(m_renderables.begin(), m_renderables.end(),
            [drawable](const Renderable& r) { return r.drawable == drawable; }),
        m_renderables.end()
    );
}

void Render::clearRenderables() {
    m_renderables.clear();
}

void Render::addParallaxLayer(const sf::Texture& texture, float speedX, float speedY,
    RenderLayer layer, bool repeat) {
    ParallaxLayer parallaxLayer;
    parallaxLayer.sprite.setTexture(texture);
    parallaxLayer.speedFactorX = speedX;
    parallaxLayer.speedFactorY = speedY;
    parallaxLayer.repeat = repeat;
    parallaxLayer.renderLayer = layer;

    m_parallaxLayers.push_back(parallaxLayer);
}

void Render::clearParallaxLayers() {
    m_parallaxLayers.clear();
}

void Render::setParallaxOffset(const sf::Vector2f& offset) {
    m_parallaxOffset = offset;
}

void Render::updateParallaxOffset(const sf::Vector2f& delta) {
    m_parallaxOffset += delta;
}

bool Render::loadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) {
    auto shader = std::make_unique<sf::Shader>();

    bool loaded = false;
    if (vertexPath.empty() && !fragmentPath.empty()) {
        loaded = shader->loadFromFile(fragmentPath, sf::Shader::Fragment);
    }
    else if (!vertexPath.empty() && fragmentPath.empty()) {
        loaded = shader->loadFromFile(vertexPath, sf::Shader::Vertex);
    }
    else if (!vertexPath.empty() && !fragmentPath.empty()) {
        loaded = shader->loadFromFile(vertexPath, fragmentPath);
    }

    if (loaded) {
        m_shaders[name] = std::move(shader);
        return true;
    }
    else {
        std::cerr << "Failed to load shader: " << name << std::endl;
        return false;
    }
}

void Render::setShader(const std::string& name) {
    auto it = m_shaders.find(name);
    if (it != m_shaders.end()) {
        m_currentShader = it->second.get();
        m_usePostProcessing = true;
    }
    else {
        std::cerr << "Shader not found: " << name << std::endl;
    }
}

void Render::clearShader() {
    m_currentShader = nullptr;
    m_usePostProcessing = false;
}

void Render::setPostProcessing(bool enable) {
    m_usePostProcessing = enable;
}

void Render::shakeScreen(float intensity, float duration) {
    if (m_camera) {
        m_camera->shake(intensity, duration);
    }
}

void Render::update(float deltaTime) {

}

void Render::render() {
    if (!m_window || !m_camera) return;

    sortRenderables();

    sf::RenderTarget* target = m_usePostProcessing
        ? static_cast<sf::RenderTarget*>(&m_renderTexture)
        : static_cast<sf::RenderTarget*>(m_window);


    target->clear(sf::Color(20, 20, 20));

    m_camera->applyTo(*m_window);
    sf::View currentView = m_window->getView();
    target->setView(currentView);

    drawParallaxLayers();

    for (const auto& renderable : m_renderables) {
        bool isVisible = true;
        if (renderable.entity) {
            isVisible = renderable.entity->isVisible();
        }
        
        if (!isVisible)
            continue;

        if (renderable.entity) {
            renderable.entity->render(*m_window);
        }
        else if (renderable.drawable) {
            target->draw(*renderable.drawable);
        }
    }




    target->setView(target->getDefaultView());

    if (m_usePostProcessing) {
        m_renderTexture.display();

        sf::Sprite finalSprite(m_renderTexture.getTexture());

        applyPostEffects();

        m_window->clear(sf::Color::Black);

        if (m_currentShader && sf::Shader::isAvailable()) {
            m_window->draw(finalSprite, m_currentShader);
        }
        else {
            m_window->draw(finalSprite);
        }
    }

    m_window->display();
}

Camera* Render::getCamera() const {
    return m_camera;
}

void Render::sortRenderables() {
    std::sort(m_renderables.begin(), m_renderables.end(),
        [](const Renderable& a, const Renderable& b) {
            if (a.layer != b.layer) {
                return static_cast<int>(a.layer) < static_cast<int>(b.layer);
            }
            return a.zOrder < b.zOrder;
        });
}

void Render::drawParallaxLayers() {
    if (!m_window || !m_camera) return;

    sf::RenderTarget* target = m_usePostProcessing
        ? static_cast<sf::RenderTarget*>(&m_renderTexture)
        : static_cast<sf::RenderTarget*>(m_window);

    sf::Vector2f cameraCenter = m_camera->getCenter();
    sf::Vector2f cameraSize = m_camera->getSize();

    for (auto& layer : m_parallaxLayers) {
        sf::Vector2f layerPos(
            cameraCenter.x * layer.speedFactorX - cameraSize.x / 2.0f + m_parallaxOffset.x * layer.speedFactorX,
            cameraCenter.y * layer.speedFactorY - cameraSize.y / 2.0f + m_parallaxOffset.y * layer.speedFactorY
        );

        sf::Vector2f textureSize(
            static_cast<float>(layer.sprite.getTexture()->getSize().x),
            static_cast<float>(layer.sprite.getTexture()->getSize().y)
        );

        if (layer.repeat) {
            int startX = static_cast<int>(layerPos.x / textureSize.x) - 1;
            int startY = static_cast<int>(layerPos.y / textureSize.y) - 1;
            int endX = static_cast<int>((layerPos.x + cameraSize.x) / textureSize.x) + 1;
            int endY = static_cast<int>((layerPos.y + cameraSize.y) / textureSize.y) + 1;

            for (int y = startY; y <= endY; ++y) {
                for (int x = startX; x <= endX; ++x) {
                    layer.sprite.setPosition(x * textureSize.x, y * textureSize.y);
                    target->draw(layer.sprite);
                }
            }
        }
        else {
            layer.sprite.setPosition(layerPos);
            target->draw(layer.sprite);
        }
    }
}

void Render::applyPostEffects() {

}