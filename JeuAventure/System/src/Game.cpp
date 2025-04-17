#include "Game.h"
#include "RessourceManager.h"
#include "InputManager.h"
#include "EventSystem.h"
#include "SaveSystem.h"
#include <iostream>

Game::Game() :
    m_isRunning(false),
    m_isPaused(false),
    m_windowWidth(800),
    m_windowHeight(400),
    m_windowTitle("Platformer Game"),
    m_isFullscreen(false),
    m_targetFPS(60),
    m_timeStep(1.0f / 60.0f),
    m_currentFPS(0.0f),
    m_fpsFrameCount(0),
    m_accumulatedTime(sf::Time::Zero)
{

}

Game::~Game() {
    InputManager::cleanup();
    EventSystem::cleanup();
    SaveSystem::cleanup();
}

void Game::initialize(const std::string& title, unsigned int width, unsigned int height, bool fullscreen) {
    m_windowTitle = title;
    m_windowWidth = width;
    m_windowHeight = height;
    m_isFullscreen = fullscreen;

    if (m_isFullscreen) {
        m_window = std::make_unique<sf::RenderWindow>(
            sf::VideoMode(m_windowWidth, m_windowHeight),
            m_windowTitle,
            sf::Style::Fullscreen
        );
    }
    else {
        m_window = std::make_unique<sf::RenderWindow>(
            sf::VideoMode(m_windowWidth, m_windowHeight),
            m_windowTitle,
            sf::Style::Titlebar | sf::Style::Close
        );
    }

    m_window->setVerticalSyncEnabled(true);

    m_window->setFramerateLimit(m_targetFPS);

    initializeSystems();

    m_isRunning = true;

    m_fpsClock.restart();
}

void Game::initializeSystems() {
    m_stateManager = std::make_unique<StateManager>(*this);

    SaveSystem::getInstance()->setSavePath("./saves/");

    EventSystem::getInstance()->addEventListener("QuitGame", [this](const std::map<std::string, std::any>&) {
        this->quit();
        });

    EventSystem::getInstance()->addEventListener("PauseGame", [this](const std::map<std::string, std::any>&) {
        this->pause();
        });

    EventSystem::getInstance()->addEventListener("ResumeGame", [this](const std::map<std::string, std::any>&) {
        this->resume();
        });

    EventSystem::getInstance()->addEventListener("ToggleFullscreen", [this](const std::map<std::string, std::any>&) {
        this->toggleFullscreen();
        });
}

void Game::run() {
    m_stateManager->pushState(StateType::Menu);

    while (m_isRunning && m_window->isOpen()) {
        m_elapsedTime = m_gameClock.restart();

        m_accumulatedTime += m_elapsedTime;

        processEvents();

        while (m_accumulatedTime.asSeconds() >= m_timeStep) {
            if (!m_isPaused) {
                update(m_timeStep);
            }
            m_accumulatedTime -= sf::seconds(m_timeStep);
        }

        render();

        updateFPS();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (m_window->pollEvent(event)) {
        m_stateManager->handleEvent(event);

        switch (event.type) {
        case sf::Event::Closed:
            quit();
            break;
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::F11) {
                toggleFullscreen();
            }
            break;
        default:
            break;
        }
    }

    InputManager::getInstance()->update(*m_window);
}

void Game::update(float dt) {
    m_stateManager->update(dt);
}

void Game::render() {
    m_window->clear(sf::Color(20, 20, 50));

    if (m_stateManager->getActiveStateType() == StateType::Game) {
        m_window->setView(m_window->getDefaultView());
    }
    else {
        m_window->setView(m_window->getDefaultView());
    }

    m_stateManager->render(*m_window);
    m_window->display();
}


void Game::pause() {
    if (!m_isPaused && m_stateManager->getActiveStateType() == StateType::Game) {
        m_isPaused = true;
        m_stateManager->pushState(StateType::Pause);
    }
}

void Game::resume() {
    if (m_isPaused && m_stateManager->getActiveStateType() == StateType::Pause) {
        m_isPaused = false;
        m_stateManager->popState();
    }
}

void Game::quit() {
    m_isRunning = false;
    m_window->close();
}

void Game::toggleFullscreen() {
    m_isFullscreen = !m_isFullscreen;

    sf::Vector2i position = m_window->getPosition();

    m_window->close();

    if (m_isFullscreen) {
        m_window = std::make_unique<sf::RenderWindow>(
            sf::VideoMode(m_windowWidth, m_windowHeight),
            m_windowTitle,
            sf::Style::Fullscreen
        );
    }
    else {
        m_window = std::make_unique<sf::RenderWindow>(
            sf::VideoMode(m_windowWidth, m_windowHeight),
            m_windowTitle,
            sf::Style::Titlebar | sf::Style::Close
        );
        m_window->setPosition(position);
    }

    m_window->setVerticalSyncEnabled(true);
    m_window->setFramerateLimit(m_targetFPS);
}

void Game::setWindowTitle(const std::string& title) {
    m_windowTitle = title;
    m_window->setTitle(m_windowTitle);
}

void Game::setWindowSize(unsigned int width, unsigned int height) {
    if (m_isFullscreen) return;

    m_windowWidth = width;
    m_windowHeight = height;
    m_window->setSize(sf::Vector2u(m_windowWidth, m_windowHeight));
}

sf::RenderWindow& Game::getWindow() {
    return *m_window;
}

StateManager& Game::getStateManager() {
    return *m_stateManager;
}

unsigned int Game::getWindowWidth() const {
    return m_windowWidth;
}

unsigned int Game::getWindowHeight() const {
    return m_windowHeight;
}

bool Game::isPaused() const {
    return m_isPaused;
}

void Game::setTargetFPS(unsigned int fps) {
    m_targetFPS = fps;
    m_timeStep = 1.0f / static_cast<float>(fps);
    m_window->setFramerateLimit(m_targetFPS);
}

unsigned int Game::getTargetFPS() const {
    return m_targetFPS;
}

float Game::getFPS() const {
    return m_currentFPS;
}

void Game::updateFPS() {
    m_fpsFrameCount++;

    if (m_fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
        m_currentFPS = static_cast<float>(m_fpsFrameCount) / m_fpsClock.getElapsedTime().asSeconds();
        m_fpsFrameCount = 0;
        m_fpsClock.restart();
    }
}

bool Game::isFullscreen() const {
    return m_isFullscreen;
}

//#include "Game.h"
//
//Quadtree::Quadtree(Boundary boundary, int capacity)
//    : boundary(boundary), capacity(capacity), divided(false) {
//}
//
//void Quadtree::subdivide() {
//    float x = boundary.x;
//    float y = boundary.y;
//    float w = boundary.w / 2.0f;
//    float h = boundary.h / 2.0f;
//
//    Boundary nw(x, y, w, h);
//    northwest = std::make_unique<Quadtree>(nw, capacity);
//
//    Boundary ne(x + w, y, w, h);
//    northeast = std::make_unique<Quadtree>(ne, capacity);
//
//    Boundary sw(x, y + h, w, h);
//    southwest = std::make_unique<Quadtree>(sw, capacity);
//
//    Boundary se(x + w, y + h, w, h);
//    southeast = std::make_unique<Quadtree>(se, capacity);
//
//    divided = true;
//
//    for (const auto& p : points) {
//        if (northwest->boundary.contains(p)) northwest->insert(p);
//        else if (northeast->boundary.contains(p)) northeast->insert(p);
//        else if (southwest->boundary.contains(p)) southwest->insert(p);
//        else if (southeast->boundary.contains(p)) southeast->insert(p);
//    }
//
//    points.clear();
//}
//
//bool Quadtree::insert(const Point& point) {
//    if (!boundary.contains(point)) {
//        return false;
//    }
//
//    if (points.size() < capacity && !divided) {
//        points.push_back(point);
//        return true;
//    }
//
//    if (!divided) {
//        subdivide();
//    }
//
//    if (northwest->insert(point)) return true;
//    if (northeast->insert(point)) return true;
//    if (southwest->insert(point)) return true;
//    if (southeast->insert(point)) return true;
//
//    return false;
//}
//
//void Quadtree::query(const Boundary& range, std::vector<Point>& found) const {
//    if (!boundary.intersects(range)) {
//        return;
//    }
//
//    for (const auto& p : points) {
//        if (range.contains(p)) {
//            found.push_back(p);
//        }
//    }
//
//    if (divided) {
//        northwest->query(range, found);
//        northeast->query(range, found);
//        southwest->query(range, found);
//        southeast->query(range, found);
//    }
//}
//
//bool Quadtree::findLeafNodeAt(const Point& point, Boundary& leafBoundary)
//{
//    if (!boundary.contains(point)) {
//        return false;
//    }
//
//    if (!divided) 
//    {
//        leafBoundary = boundary;
//        return true;
//    }
//
//    if (northwest->findLeafNodeAt(point, leafBoundary)) return true;
//    if (northeast->findLeafNodeAt(point, leafBoundary)) return true;
//    if (southwest->findLeafNodeAt(point, leafBoundary)) return true;
//    if (southeast->findLeafNodeAt(point, leafBoundary)) return true;
//
//    return false;
//}
//
//void Quadtree::draw(sf::RenderWindow& window, const sf::Color& color,
//    const Boundary& highlightCell,
//    const std::vector<Boundary>& adjacentCells) const
//{
//    sf::RectangleShape rect;
//    rect.setPosition(boundary.x, boundary.y);
//    rect.setSize(sf::Vector2f(boundary.w, boundary.h));
//    rect.setFillColor(sf::Color::Transparent);
//
//    bool isHighlighted = boundary.equals(highlightCell);
//    bool isAdjacent = false;
//
//    for (const auto& adjCell : adjacentCells) 
//    {
//        if (boundary.equals(adjCell)) 
//        {
//            isAdjacent = true;
//            break;
//        }
//    }
//
//    if (isHighlighted) 
//    {
//        sf::RectangleShape highlightRect;
//        highlightRect.setPosition(boundary.x, boundary.y);
//        highlightRect.setSize(sf::Vector2f(boundary.w, boundary.h));
//        highlightRect.setFillColor(sf::Color(100, 100, 255, 100));
//        window.draw(highlightRect);
//        rect.setOutlineColor(sf::Color::Blue);
//        rect.setOutlineThickness(1.5f);
//    }
//    else if (isAdjacent) 
//    {
//        sf::RectangleShape adjRect;
//        adjRect.setPosition(boundary.x, boundary.y);
//        adjRect.setSize(sf::Vector2f(boundary.w, boundary.h));
//        adjRect.setFillColor(sf::Color(100, 100, 255, 100));
//        window.draw(adjRect);
//        rect.setOutlineColor(sf::Color(150, 200, 255));
//        rect.setOutlineThickness(1.2f);
//    }
//    else {
//        rect.setOutlineColor(color);
//        rect.setOutlineThickness(1.0f);
//    }
//
//    window.draw(rect);
//
//    if (divided) 
//    {
//        northwest->draw(window, color, highlightCell, adjacentCells);
//        northeast->draw(window, color, highlightCell, adjacentCells);
//        southwest->draw(window, color, highlightCell, adjacentCells);
//        southeast->draw(window, color, highlightCell, adjacentCells);
//    }
//}
//
//void Quadtree::getAllLeafNodes(std::vector<Boundary>& leafBoundaries)
//{
//    if (!divided) 
//    {
//        leafBoundaries.push_back(boundary);
//    }
//    else 
//    {
//        northwest->getAllLeafNodes(leafBoundaries);
//        northeast->getAllLeafNodes(leafBoundaries);
//        southwest->getAllLeafNodes(leafBoundaries);
//        southeast->getAllLeafNodes(leafBoundaries);
//    }
//}
//
//void Quadtree::findAdjacentLeafNodes(const Boundary& cellBoundary, std::vector<Boundary>& adjacentCells)
//{
//    if (!boundary.intersects(Boundary(
//        cellBoundary.x - 1, cellBoundary.y - 1,
//        cellBoundary.w + 2, cellBoundary.h + 2))) {
//        return;
//    }
//
//    if (!divided) 
//    {
//        if (!boundary.equals(cellBoundary) && boundary.isInContactWith(cellBoundary)) 
//        {
//            adjacentCells.push_back(boundary);
//        }
//    }
//    else 
//    {
//        northwest->findAdjacentLeafNodes(cellBoundary, adjacentCells);
//        northeast->findAdjacentLeafNodes(cellBoundary, adjacentCells);
//        southwest->findAdjacentLeafNodes(cellBoundary, adjacentCells);
//        southeast->findAdjacentLeafNodes(cellBoundary, adjacentCells);
//    }
//}