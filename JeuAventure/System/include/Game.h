#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

struct Point
{
    float x;
    float y;

    Point() : x(0), y(0) {}
    Point(float x, float y) : x(x), y(y) {}
};

struct Boundary
{
    float x, y, w, h;

    Boundary() : x(0), y(0), w(0), h(0) {}
    Boundary(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}

    bool contains(const Point& point) const
	{
        return (point.x >= x && point.x < x + w &&
            point.y >= y && point.y < y + h);
    }

    bool intersects(const Boundary& other) const
	{
        return !(x > other.x + other.w ||
            x + w < other.x ||
            y > other.y + other.h ||
            y + h < other.y);
    }
};

class Quadtree
{
private:
    Boundary boundary;
    int capacity;
    std::vector<Point> points;
    bool divided;

    std::unique_ptr<Quadtree> northwest;
    std::unique_ptr<Quadtree> northeast;
    std::unique_ptr<Quadtree> southwest;
    std::unique_ptr<Quadtree> southeast;

    void subdivide();

public:
    Quadtree(Boundary boundary, int capacity);
    ~Quadtree() = default;

    bool insert(const Point& point);
    void query(const Boundary& range, std::vector<Point>& found) const;
    void draw(sf::RenderWindow& window, const sf::Color& color) const;
};