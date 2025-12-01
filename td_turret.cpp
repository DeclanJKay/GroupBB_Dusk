// td_turret.cpp
#include "td_turret.hpp"
#include "TDEnemy.hpp"

#include <cmath>

TDTurret::TDTurret(const sf::Vector2i& grid,
    const sf::Vector2f& worldPos,
    float tileSize,
    TurretType type)
    : _grid(grid)
    , _tileSize(tileSize)
    , _type(type)
    , _stats(get_turret_stats(type))
{
    _shape.setSize({ tileSize, tileSize });
    _shape.setPosition(worldPos);
    _shape.setFillColor(_stats.color);
}

// Decide if this turret fires a bullet this frame
bool TDTurret::update(float dt,
    std::vector<TDEnemy>& enemies,
    sf::Vector2f& outBulletPos,
    sf::Vector2f& outBulletDir)
{
    // Cooldown
    if (_cooldown > 0.f)
    {
        _cooldown -= dt;
        if (_cooldown < 0.f) _cooldown = 0.f;
        return false;
    }

    // Range in world units
    const float rangePixels = _stats.rangeTiles * _tileSize;
    const float rangeSq = rangePixels * rangePixels;

    // Centre of this turret tile
    sf::Vector2f turretCenter =
        _shape.getPosition() + 0.5f * _shape.getSize();

    // Find closest enemy in range
    TDEnemy* best = nullptr;
    float    bestDistSq = rangeSq;

    for (auto& e : enemies)
    {
        if (e.isDead()) continue;

        sf::Vector2f diff = e.getPosition() - turretCenter;
        float d2 = diff.x * diff.x + diff.y * diff.y;
        if (d2 < bestDistSq)
        {
            bestDistSq = d2;
            best = &e;
        }
    }

    // No enemy in range = idle colour and no shot
    if (!best)
    {
        _shape.setFillColor(_stats.color);
        return false;
    }

    // Direction towards target enemy
    sf::Vector2f dir = best->getPosition() - turretCenter;
    float lenSq = dir.x * dir.x + dir.y * dir.y;
    if (lenSq <= 0.0001f)
    {
        dir = { 1.f, 0.f };
    }
    else
    {
        float len = std::sqrt(lenSq);
        dir /= len;
    }

    outBulletPos = turretCenter;
    outBulletDir = dir;

    // Use turret-specific fire interval
    _cooldown = _stats.fireInterval;

    // Briefly tint when firing (slightly brighter)
    _shape.setFillColor(sf::Color(
        std::min<int>(_stats.color.r + 40, 255),
        std::min<int>(_stats.color.g + 40, 255),
        std::min<int>(_stats.color.b + 40, 255)
    ));

    return true;
}

void TDTurret::render(sf::RenderWindow& window) const
{
    window.draw(_shape);
}
