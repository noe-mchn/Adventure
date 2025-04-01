#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include "Game.h"

const int width = 800;
const int height = 800;
const int tailleOfTheGrille = 8;
const float widthOfTheDecoupe = width / tailleOfTheGrille;
const float heightOfTheDecoupe = height / tailleOfTheGrille;
const int nbOfPoint = 100;

int main()
{
    sf::RenderWindow window(sf::VideoMode(width, height), "Test Bouffon");
    window.setFramerateLimit(60);

    Boundary boundary(0, 0, width, height);
    Quadtree quadtree(boundary, 1);

    std::vector<sf::Vector2f> points;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distX(0.0f, width);
    std::uniform_real_distribution<float> distY(0.0f, height);

    for (int i = 0; i < nbOfPoint; i++)
    {
        float x = distX(gen);
        float y = distY(gen);
        points.push_back(sf::Vector2f(x, y));
        quadtree.insert(Point(x, y));
    }

    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event)) 
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePos);

        int cellX = static_cast<int>(mouseWorldPos.x / widthOfTheDecoupe);
        int cellY = static_cast<int>(mouseWorldPos.y / heightOfTheDecoupe);

        window.clear(sf::Color::Black);

        quadtree.draw(window, sf::Color::White);

        for (int i = 0; i <= tailleOfTheGrille; i++)
        {
            sf::Vertex verticalLine[] =
            {
                sf::Vertex(sf::Vector2f(i * widthOfTheDecoupe, 0), sf::Color::Green),
                sf::Vertex(sf::Vector2f(i * widthOfTheDecoupe, height), sf::Color::Green)
            };
            window.draw(verticalLine, 2, sf::Lines);

            sf::Vertex horizontalLine[] = 
            {
                sf::Vertex(sf::Vector2f(0, i * heightOfTheDecoupe), sf::Color::Green),
                sf::Vertex(sf::Vector2f(width, i * heightOfTheDecoupe), sf::Color::Green)
            };
            window.draw(horizontalLine, 2, sf::Lines);
        }

        if (cellX >= 0 && cellX < tailleOfTheGrille && cellY >= 0 && cellY < tailleOfTheGrille)
        {
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int nx = cellX + dx;
                    int ny = cellY + dy;

                    if (nx >= 0 && nx < tailleOfTheGrille && ny >= 0 && ny < tailleOfTheGrille)
                    {
                        sf::RectangleShape cell;
                        cell.setSize(sf::Vector2f(widthOfTheDecoupe, heightOfTheDecoupe));
                        cell.setPosition(nx * widthOfTheDecoupe, ny * heightOfTheDecoupe);
                        cell.setFillColor(sf::Color(100, 100, 255, 100));
                        window.draw(cell);

                        Boundary cellBounds(nx * widthOfTheDecoupe, ny * heightOfTheDecoupe, widthOfTheDecoupe, heightOfTheDecoupe);
                        std::vector<Point> pointsInCell;
                        quadtree.query(cellBounds, pointsInCell);

                        for (const auto& p : pointsInCell) 
                        {
                            sf::CircleShape point(5);
                            point.setFillColor(sf::Color::Yellow);
                            point.setPosition(p.x - 5, p.y - 5);
                            window.draw(point);
                        }
                    }
                }
            }
        }

        for (const auto& p : points) {
            sf::CircleShape point(3);
            point.setFillColor(sf::Color::White);
            point.setPosition(p.x - 3, p.y - 3);
            window.draw(point);
        }

        window.display();
    }

    return 0;
}