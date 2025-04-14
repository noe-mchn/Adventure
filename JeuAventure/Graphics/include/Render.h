#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "Camera.h"

class Entity;
class Level;

enum class RenderLayer {
    Background,
    BackParallax,
    Midground,
    Entities,
    Foreground,
    ForeParallax,
    UI
};

struct Renderable {
    Entity* entity;
    sf::Drawable* drawable;
    RenderLayer layer;
    int zOrder;
    bool visible;

    Renderable() : entity(nullptr), drawable(nullptr), layer(RenderLayer::Midground), zOrder(0), visible(true) {}
};

struct ParallaxLayer {
    sf::Sprite sprite;
    float speedFactorX;
    float speedFactorY;
    bool repeat;
    RenderLayer renderLayer;
};

class Render {
private:
    sf::RenderWindow* m_window;
    Camera* m_camera;
    Level* m_currentLevel;

    std::vector<Renderable> m_renderables;

    std::vector<ParallaxLayer> m_parallaxLayers;
    sf::Vector2f m_parallaxOffset;

    sf::RenderTexture m_renderTexture;
    sf::Shader* m_currentShader;
    bool m_usePostProcessing;

    float m_fadeAlpha;
    sf::Color m_fadeColor;
    float m_vignetteIntensity;

    std::unordered_map<std::string, std::unique_ptr<sf::Shader>> m_shaders;

    void sortRenderables();

    void drawParallaxLayers();

    void drawDebugInfo();

    void applyPostEffects();

public:
    Render();
    ~Render();

    void initialize(sf::RenderWindow* window, Camera* camera);

    void setCurrentLevel(Level* level);
    Level* getCurrentLevel() const;

    void addEntity(Entity* entity, RenderLayer layer = RenderLayer::Entities, int zOrder = 0);
    void addDrawable(sf::Drawable* drawable, RenderLayer layer = RenderLayer::Midground, int zOrder = 0);
    void removeEntity(Entity* entity);
    void removeDrawable(sf::Drawable* drawable);
    void clearRenderables();

    void addParallaxLayer(const sf::Texture& texture, float speedX, float speedY,
        RenderLayer layer = RenderLayer::BackParallax, bool repeat = true);
    void clearParallaxLayers();
    void setParallaxOffset(const sf::Vector2f& offset);
    void updateParallaxOffset(const sf::Vector2f& delta);

    bool loadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
    void setShader(const std::string& name);
    void clearShader();
    void setPostProcessing(bool enable);

    void shakeScreen(float intensity, float duration);

    void update(float deltaTime);
    void render();

    void showDebugInfo(bool show);
    bool isDebugInfoVisible() const;

    int getDrawCalls() const;
    int getVisibleEntities() const;

    Camera* getCamera() const;
};
