#include "td_turret.hpp"
#include "TDEnemy.hpp"

#include <cmath>

TDTurret::TDTurret(const sf::Vector2i& grid,
    const sf::Vector2f& worldPos,
    float tileSize)
    : _grid(grid),
    _tileSize(tileSize)
{
    _shape.setSize({ tileSize, tileSize });
    _shape.setPosition(worldPos);
    _shape.setFillColor(sf::Color(0, 200, 255));
}

// Decide if this turret fires a bullet this frame
bool TDTurret::update(float dt,
    std::vector<TDEnemy>& enemies,
    sf::Vector2f& outBulletPos,
    sf::Vector2f& outBulletDir)
{
    if (_cooldown > 0.f) {
        _cooldown -= dt;
        if (_cooldown < 0.f) _cooldown = 0.f;
        return false;
    }

    const float rangePixels = kRangeTiles * _tileSize;
    const float rangeSq = rangePixels * rangePixels;

    // Centre of this turret tile
    sf::Vector2f turretCenter =
        _shape.getPosition() + 0.5f * _shape.getSize();

    // Find closest enemy in range
    TDEnemy* best = nullptr;
    float bestDistSq = rangeSq;

    for (auto& e : enemies) {
        if (e.isDead()) continue;

        sf::Vector2f diff = e.getPosition() - turretCenter;
        float d2 = diff.x * diff.x + diff.y * diff.y;
        if (d2 < bestDistSq) {
            bestDistSq = d2;
            best = &e;
        }
    }

    if (!best) {
        _shape.setFillColor(sf::Color(0, 200, 255));
        return false;
    }

    // Direction towards target enemy
    sf::Vector2f dir = best->getPosition() - turretCenter;
    float lenSq = dir.x * dir.x + dir.y * dir.y;
    if (lenSq <= 0.0001f) {
        dir = { 1.f, 0.f };
    }
    else {
        float len = std::sqrt(lenSq);
        dir /= len;
    }

    outBulletPos = turretCenter;
    outBulletDir = dir;

    _cooldown = kFireInterval;
    _shape.setFillColor(sf::Color(0, 230, 255)); // “just fired” tint

    return true;
}

void TDTurret::render(sf::RenderWindow& window) const {
    window.draw(_shape);
}
