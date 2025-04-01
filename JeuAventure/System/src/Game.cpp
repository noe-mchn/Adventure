#include "Game.h"

Quadtree::Quadtree(Boundary boundary, int capacity)
    : boundary(boundary), capacity(capacity), divided(false)
{
}

void Quadtree::subdivide()
{
    float x = boundary.x;
    float y = boundary.y;
    float w = boundary.w / 2.0f;
    float h = boundary.h / 2.0f;

    Boundary nw(x, y, w, h);
    northwest = std::make_unique<Quadtree>(nw, capacity);

    Boundary ne(x + w, y, w, h);
    northeast = std::make_unique<Quadtree>(ne, capacity);

    Boundary sw(x, y + h, w, h);
    southwest = std::make_unique<Quadtree>(sw, capacity);

    Boundary se(x + w, y + h, w, h);
    southeast = std::make_unique<Quadtree>(se, capacity);

    divided = true;

    for (const auto& p : points) 
    {
        if (northwest->boundary.contains(p)) northwest->insert(p);
        else if (northeast->boundary.contains(p)) northeast->insert(p);
        else if (southwest->boundary.contains(p)) southwest->insert(p);
        else if (southeast->boundary.contains(p)) southeast->insert(p);
    }

    points.clear();
}

bool Quadtree::insert(const Point& point)
{
    if (!boundary.contains(point)) 
    {
        return false;
    }

    if (points.size() < capacity && !divided) 
    {
        points.push_back(point);
        return true;
    }

    if (!divided) 
    {
        subdivide();
    }

    if (northwest->insert(point)) return true;
    if (northeast->insert(point)) return true;
    if (southwest->insert(point)) return true;
    if (southeast->insert(point)) return true;

    return false;
}

void Quadtree::query(const Boundary& range, std::vector<Point>& found) const
{
    if (!boundary.intersects(range)) 
    {
        return;
    }

    for (const auto& p : points) 
    {
        if (range.contains(p)) {
            found.push_back(p);
        }
    }

    if (divided) 
    {
        northwest->query(range, found);
        northeast->query(range, found);
        southwest->query(range, found);
        southeast->query(range, found);
    }
}

void Quadtree::draw(sf::RenderWindow& window, const sf::Color& color) const
{
    sf::RectangleShape rect;
    rect.setPosition(boundary.x, boundary.y);
    rect.setSize(sf::Vector2f(boundary.w, boundary.h));
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(color);
    rect.setOutlineThickness(1.0f);
    window.draw(rect);

    if (divided) 
    {
        northwest->draw(window, color);
        northeast->draw(window, color);
        southwest->draw(window, color);
        southeast->draw(window, color);
    }
}