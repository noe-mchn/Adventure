#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include "StateManager.h"

class Game {
private:
    // Window and rendering
    std::unique_ptr<sf::RenderWindow> m_window;
    sf::Clock m_gameClock;
    sf::Time m_elapsedTime;
    sf::Time m_accumulatedTime;

    // Game state
    bool m_isRunning;
    bool m_isPaused;

    // State manager
    std::unique_ptr<StateManager> m_stateManager;

    // Window settings
    unsigned int m_windowWidth;
    unsigned int m_windowHeight;
    std::string m_windowTitle;
    bool m_isFullscreen;

    // Frame rate
    unsigned int m_targetFPS;
    float m_timeStep;  // Fixed time step for updates

    // FPS calculation
    float m_currentFPS;
    sf::Clock m_fpsClock;
    int m_fpsFrameCount;

    // Initialize systems
    void initializeSystems();

public:
    // Constructor/Destructor
    Game();
    ~Game();

    // Init and run game
    void initialize(const std::string& title, unsigned int width, unsigned int height, bool fullscreen = false);
    void run();

    // Game loop methods
    void processEvents();
    void update(float dt);
    void render();

    // Game state methods
    void pause();
    void resume();
    void quit();

    // Window management
    void toggleFullscreen();
    void setWindowTitle(const std::string& title);
    void setWindowSize(unsigned int width, unsigned int height);

    // Getters
    sf::RenderWindow& getWindow();
    StateManager& getStateManager();
    unsigned int getWindowWidth() const;
    unsigned int getWindowHeight() const;
    bool isPaused() const;

    // FPS control
    void setTargetFPS(unsigned int fps);
    unsigned int getTargetFPS() const;
    float getFPS() const;
    void updateFPS();
};

//#pragma once
//#include <SFML/Graphics.hpp>
//#include <vector>
//#include <memory>
//
//struct Point {
//    float x;
//    float y;
//
//    Point() : x(0), y(0) {}
//    Point(float x, float y) : x(x), y(y) {}
//};
//
//struct Boundary {
//    float x, y, w, h;
//
//    Boundary() : x(0), y(0), w(0), h(0) {}
//    Boundary(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
//
//    bool contains(const Point& point) const {
//        return (point.x >= x && point.x < x + w &&
//            point.y >= y && point.y < y + h);
//    }
//
//    bool intersects(const Boundary& other) const {
//        return !(x > other.x + other.w ||
//            x + w < other.x ||
//            y > other.y + other.h ||
//            y + h < other.y);
//    }
//
//    bool equals(const Boundary& other) const {
//        return (x == other.x && y == other.y &&
//            w == other.w && h == other.h);
//    }
//
//    bool isAdjacentTo(const Boundary& other) const
//	{
//        bool horizontalTouch = (x + w == other.x || other.x + other.w == x) &&
//            !((y + h <= other.y) || (other.y + other.h <= y));
//
//        bool verticalTouch = (y + h == other.y || other.y + other.h == y) &&
//            !((x + w <= other.x) || (other.x + other.w <= x));
//
//        return horizontalTouch || verticalTouch;
//    }
//
//    bool isInContactWith(const Boundary& other) const
//	{
//        return (x <= other.x + other.w && x + w >= other.x &&
//            y <= other.y + other.h && y + h >= other.y) &&
//            !(x < other.x + other.w && x + w > other.x &&
//                y < other.y + other.h && y + h > other.y);
//    }
//};
//
//class Quadtree {
//private:
//    Boundary boundary;
//    int capacity;
//    std::vector<Point> points;
//    bool divided;
//
//    std::unique_ptr<Quadtree> northwest;
//    std::unique_ptr<Quadtree> northeast;
//    std::unique_ptr<Quadtree> southwest;
//    std::unique_ptr<Quadtree> southeast;
//
//    void subdivide();
//
//public:
//    Quadtree(Boundary boundary, int capacity);
//    ~Quadtree() = default;
//
//    bool insert(const Point& point);
//    void query(const Boundary& range, std::vector<Point>& found) const;
//    void draw(sf::RenderWindow& window, const sf::Color& color,
//        const Boundary& highlightCell = Boundary(),
//        const std::vector<Boundary>& adjacentCells = std::vector<Boundary>()) const;
//    bool findLeafNodeAt(const Point& point, Boundary& leafBoundary);
//    void getAllLeafNodes(std::vector<Boundary>& leafBoundaries);
//    void findAdjacentLeafNodes(const Boundary& cellBoundary, std::vector<Boundary>& adjacentCells);
//};