#include "CollisionManager.h"
#include <algorithm>

CollisionManager* CollisionManager::s_instance = nullptr;

CollisionManager::CollisionManager()
    : m_debugDraw(false), m_grid(100)
{
}

CollisionManager* CollisionManager::getInstance() {
    if (s_instance == nullptr) {
        s_instance = new CollisionManager();
    }
    return s_instance;
}

void CollisionManager::cleanup() {
    if (s_instance != nullptr) {
        delete s_instance;
        s_instance = nullptr;
    }
}

void CollisionManager::registerCollider(Collider* collider) {
    if (!collider) return;

    auto it = std::find(m_colliders.begin(), m_colliders.end(), collider);
    if (it == m_colliders.end()) {
        m_colliders.push_back(collider);
    }
}

void CollisionManager::unregisterCollider(Collider* collider) {
    if (!collider) return;

    auto it1 = m_activeCollisions.begin();
    while (it1 != m_activeCollisions.end()) {
        if (it1->a == collider || it1->b == collider) {
            if (it1->a != collider) it1->a->onCollision(it1->b, false);
            if (it1->b != collider) it1->b->onCollision(it1->a, false);
            it1 = m_activeCollisions.erase(it1);
        }
        else {
            ++it1;
        }
    }

    auto it2 = std::find(m_colliders.begin(), m_colliders.end(), collider);
    if (it2 != m_colliders.end()) {
        m_colliders.erase(it2);
    }
}

void CollisionManager::checkCollisions() {
    m_grid.clear();

    for (auto* collider : m_colliders) {
        if (collider && collider->isEnabled()) {
            m_grid.addCollider(collider);
        }
    }

    std::set<CollisionPair> currentCollisions;

    for (auto* collider : m_colliders) {
        if (!collider->isEnabled()) continue;

        std::vector<Collider*> potentialColliders = m_grid.getPotentialColliders(collider);

        for (auto* other : potentialColliders) {
            if (!other->isEnabled()) continue;

            if (!collider->shouldCollideWith(other->getCollisionLayer()) ||
                !other->shouldCollideWith(collider->getCollisionLayer())) {
                continue;
            }

            CollisionPair pair = (collider < other) ?
                CollisionPair(collider, other) :
                CollisionPair(other, collider);

            if (collider->checkCollision(other)) {
                currentCollisions.insert(pair);

                if (m_activeCollisions.find(pair) == m_activeCollisions.end()) {
                    collider->onCollision(other, true);
                    other->onCollision(collider, true);
                }
            }
        }
    }

    for (const auto& pair : m_activeCollisions) {
        if (currentCollisions.find(pair) == currentCollisions.end()) {
            pair.a->onCollision(pair.b, false);
            pair.b->onCollision(pair.a, false);
        }
    }

    m_activeCollisions = std::move(currentCollisions);
}

void CollisionManager::setDebugDraw(bool debug) {
    m_debugDraw = debug;
}

bool CollisionManager::isDebugDrawEnabled() const {
    return m_debugDraw;
}

void CollisionManager::debugDraw(sf::RenderWindow& window) {
    if (!m_debugDraw) return;

    for (auto* collider : m_colliders) {
        if (collider && collider->isEnabled()) {
            collider->debugDraw(window);
        }
    }
}
