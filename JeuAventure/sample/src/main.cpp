#include "Application.h"
#include "Game.h"
#include "StateManager.h"
#include "MenuState.h"
#include "GameState.h"
#include "PauseState.h"
#include "GameOverState.h"
#include "RessourceManager.h"
#include "EventSystem.h"
#include "InputManager.h"
#include <iostream>

int main(int argc, char** argv) {
    try {
        std::cout << "Starting Platformer Game..." << std::endl;

        Application* app = Application::getInstance();

        app->initialize(argc, argv);

        StateManager* stateManager = app->getStateManager();

        stateManager->registerState<MenuState>(StateType::Menu);
        stateManager->registerState<GameState>(StateType::Game);
        stateManager->registerState<PauseState>(StateType::Pause);
        stateManager->registerState<GameOverState>(StateType::GameOver);

        stateManager->pushState(StateType::Menu);

        EventSystem* eventSystem = app->getEventSystem();

        eventSystem->addEventListener("GameCompleted", [](const std::map<std::string, std::any>& params) {
            std::cout << "Game completed! Showing end screen." << std::endl;
            });

        eventSystem->addEventListener("FatalError", [app](const std::map<std::string, std::any>& params) {
            std::string errorMessage = "Unknown error";
            if (params.count("message")) {
                errorMessage = std::any_cast<std::string>(params.at("message"));
            }
            std::cerr << "FATAL ERROR: " << errorMessage << std::endl;
            app->quit(1);
            });

        RessourceManager* resources = app->getRessourceManager();
        resources->loadFont("default", "arial.ttf");
        resources->loadFont("title", "arial.ttf");

        InputManager* input = app->getInputManager();
        input->bindKey(InputAction::JUMP, sf::Keyboard::W);
        input->bindKey(InputAction::JUMP, sf::Keyboard::Up);
        input->bindKey(InputAction::JUMP, sf::Keyboard::Space);

        input->bindKey(InputAction::MOVE_LEFT, sf::Keyboard::A);
        input->bindKey(InputAction::MOVE_LEFT, sf::Keyboard::Left);

        input->bindKey(InputAction::MOVE_RIGHT, sf::Keyboard::D);
        input->bindKey(InputAction::MOVE_RIGHT, sf::Keyboard::Right);

        input->bindKey(InputAction::INTERACT, sf::Keyboard::E);
        input->bindKey(InputAction::DASH, sf::Keyboard::LShift);
        input->bindKey(InputAction::ATTACK, sf::Keyboard::LControl);

        input->bindKey(InputAction::PAUSE, sf::Keyboard::Escape);

        int exitCode = app->run();

        return exitCode;
    }
    catch (const std::exception& e) {
        std::cerr << "UNHANDLED EXCEPTION: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "UNKNOWN EXCEPTION OCCURRED" << std::endl;
        return 1;
    }
}


//const int WINDOW_WIDTH = 800;
//const int WINDOW_HEIGHT = 800;
//const int GRID_SIZE = 8;
//const float CELL_WIDTH = WINDOW_WIDTH / GRID_SIZE;
//const float CELL_HEIGHT = WINDOW_HEIGHT / GRID_SIZE;
//const int NUM_POINTS = 1000;
//
//
//int main()
//{
//    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Quadtree Visualization");
//    window.setFramerateLimit(60);
//
//    sf::View view = window.getDefaultView();
//    float zoomFactor = 1.0f;
//    bool isDragging = false;
//    sf::Vector2f lastMousePos;
//
//    Boundary boundary(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
//    Quadtree quadtree(boundary, 4);
//
//    std::vector<sf::Vector2f> points;
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_real_distribution<float> distX(0.0f, WINDOW_WIDTH);
//    std::uniform_real_distribution<float> distY(0.0f, WINDOW_HEIGHT);
//
//    for (int i = 0; i < NUM_POINTS; i++) {
//        float x = distX(gen);
//        float y = distY(gen);
//        points.push_back(sf::Vector2f(x, y));
//        quadtree.insert(Point(x, y));
//    }
//
//    while (window.isOpen()) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed)
//                window.close();
//
//            if (event.type == sf::Event::MouseWheelScrolled) {
//                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
//                    sf::Vector2f mouseWorldPos = window.mapPixelToCoords(sf::Vector2i(
//                        event.mouseWheelScroll.x,
//                        event.mouseWheelScroll.y
//                    ));
//
//                    float zoomAmount = 1.1f;
//                    if (event.mouseWheelScroll.delta < 0) {
//                        zoomFactor /= zoomAmount;
//                        if (zoomFactor < 0.1f) zoomFactor = 0.1f;
//                    }
//                    else {
//                        zoomFactor *= zoomAmount;
//                        if (zoomFactor > 10.0f) zoomFactor = 10.0f;
//                    }
//
//                    view.setSize(window.getDefaultView().getSize());
//                    view.zoom(1.0f / zoomFactor);
//
//                    sf::Vector2f mouseWorldPosAfter = window.mapPixelToCoords(sf::Vector2i(
//                        event.mouseWheelScroll.x,
//                        event.mouseWheelScroll.y
//                    ), view);
//
//                    view.move(mouseWorldPos - mouseWorldPosAfter);
//                    window.setView(view);
//                }
//            }
//
//            if (event.type == sf::Event::MouseButtonPressed) {
//                if (event.mouseButton.button == sf::Mouse::Middle || event.mouseButton.button == sf::Mouse::Right) {
//                    isDragging = true;
//                    lastMousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
//                }
//            }
//
//            if (event.type == sf::Event::MouseButtonReleased) {
//                if (event.mouseButton.button == sf::Mouse::Middle || event.mouseButton.button == sf::Mouse::Right) {
//                    isDragging = false;
//                }
//            }
//
//            if (event.type == sf::Event::MouseMoved) {
//                if (isDragging) {
//                    sf::Vector2f currentMousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
//                    sf::Vector2f delta = lastMousePos - currentMousePos;
//                    view.move(delta);
//                    window.setView(view);
//                    lastMousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
//                }
//            }
//        }
//
//        sf::Vector2i mousePosScreen = sf::Mouse::getPosition(window);
//        sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePosScreen);
//
//        window.clear(sf::Color::Black);
//
//        Boundary mouseCell;
//        std::vector<Point> pointsInMouseCell;
//        bool foundMouseCell = quadtree.findLeafNodeAt(Point(mouseWorldPos.x, mouseWorldPos.y), mouseCell);
//
//        std::vector<Boundary> adjacentCells;
//        std::vector<Point> pointsInAdjacentCells;
//
//        if (foundMouseCell) 
//        {
//            quadtree.findAdjacentLeafNodes(mouseCell, adjacentCells);
//
//            quadtree.query(mouseCell, pointsInMouseCell);
//
//            for (const auto& adjCell : adjacentCells) {
//                quadtree.query(adjCell, pointsInAdjacentCells);
//            }
//        }
//
//        quadtree.draw(window, sf::Color::White, mouseCell, adjacentCells);
//
//        for (int i = 0; i <= GRID_SIZE; i++) {
//            sf::Vertex verticalLine[] = {
//                sf::Vertex(sf::Vector2f(i * CELL_WIDTH, 0), sf::Color::Green),
//                sf::Vertex(sf::Vector2f(i * CELL_WIDTH, WINDOW_HEIGHT), sf::Color::Green)
//            };
//            window.draw(verticalLine, 2, sf::Lines);
//
//            sf::Vertex horizontalLine[] = {
//                sf::Vertex(sf::Vector2f(0, i * CELL_HEIGHT), sf::Color::Green),
//                sf::Vertex(sf::Vector2f(WINDOW_WIDTH, i * CELL_HEIGHT), sf::Color::Green)
//            };
//            window.draw(horizontalLine, 2, sf::Lines);
//        }
//
//        
//        float pointSize = 3.0f / zoomFactor;
//        if (pointSize < 1.0f)
//            pointSize = 1.0f;
//
//        float highlightSize = 5.0f / zoomFactor;
//        if (highlightSize < 2.0f)
//            highlightSize = 2.0f;
//
//        for (const auto& p : points) {
//            sf::CircleShape point(pointSize);
//            point.setFillColor(sf::Color::White);
//            point.setPosition(p.x - pointSize, p.y - pointSize);
//            window.draw(point);
//        }
//
//        for (const auto& p : pointsInMouseCell) {
//            sf::CircleShape point(highlightSize);
//            point.setFillColor(sf::Color::Yellow);
//            point.setPosition(p.x - highlightSize, p.y - highlightSize);
//            window.draw(point);
//        }
//
//        sf::View uiView = window.getDefaultView();
//        window.setView(uiView);
//        window.setView(view);
//
//        window.display();
//    }
//
//    return 0;
//}