#pragma once

#include "Collider.h"
#include <vector>
#include <map>
#include <set>
#include <utility>
#include <memory>

struct CollisionPair {
    Collider* a;
    Collider* b;

    CollisionPair(Collider* _a, Collider* _b) : a(_a), b(_b) {}

    bool operator<(const CollisionPair& other) const {
        if (a != other.a) return a < other.a;
        return b < other.b;
    }
};

class CollisionManager {
private:
    static CollisionManager* s_instance;

    std::vector<Collider*> m_colliders;
    std::set<CollisionPair> m_activeCollisions;

    struct Grid {
        int cellSize;
        std::map<std::pair<int, int>, std::vector<Collider*>> cells;

        Grid(int _cellSize = 100) : cellSize(_cellSize) {}

        void clear() {
            cells.clear();
        }

        void addCollider(Collider* collider) {
            sf::FloatRect bounds = collider->getBounds();

            int minCellX = static_cast<int>(bounds.left) / cellSize;
            int minCellY = static_cast<int>(bounds.top) / cellSize;
            int maxCellX = static_cast<int>(bounds.left + bounds.width) / cellSize;
            int maxCellY = static_cast<int>(bounds.top + bounds.height) / cellSize;

            for (int y = minCellY; y <= maxCellY; ++y) {
                for (int x = minCellX; x <= maxCellX; ++x) {
                    cells[{x, y}].push_back(collider);
                }
            }
        }

        std::vector<Collider*> getPotentialColliders(Collider* collider) {
            std::vector<Collider*> result;
            std::set<Collider*> uniqueColliders;

            sf::FloatRect bounds = collider->getBounds();

            int minCellX = static_cast<int>(bounds.left) / cellSize;
            int minCellY = static_cast<int>(bounds.top) / cellSize;
            int maxCellX = static_cast<int>(bounds.left + bounds.width) / cellSize;
            int maxCellY = static_cast<int>(bounds.top + bounds.height) / cellSize;

            for (int y = minCellY; y <= maxCellY; ++y) {
                for (int x = minCellX; x <= maxCellX; ++x) {
                    auto it = cells.find({ x, y });
                    if (it != cells.end()) {
                        for (Collider* other : it->second) {
                            if (other != collider && uniqueColliders.find(other) == uniqueColliders.end()) {
                                uniqueColliders.insert(other);
                                result.push_back(other);
                            }
                        }
                    }
                }
            }

            return result;
        }
    };

    Grid m_grid;
    bool m_debugDraw;

    CollisionManager();

public:
    CollisionManager(const CollisionManager&) = delete;
    CollisionManager& operator=(const CollisionManager&) = delete;

    static CollisionManager* getInstance();
    static void cleanup();

    void registerCollider(Collider* collider);
    void unregisterCollider(Collider* collider);

    void checkCollisions();

    void setDebugDraw(bool debug);
    bool isDebugDrawEnabled() const;
    void debugDraw(sf::RenderWindow& window);
};
