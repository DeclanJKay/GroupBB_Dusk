// td_turret.hpp
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "TurretStats.hpp"

class TDEnemy;

// Turret that lives on the TD grid and shoots at enemies
class TDTurret
{
public:
    // grid = tile coordinates, worldPos = top-left of tile.
    // type = what kind of turret this is
    TDTurret(const sf::Vector2i& grid,
        const sf::Vector2f& worldPos,
        float tileSize,
        TurretType type = TurretType::Basic);

    // Update cooldown and target enemies.
    // If it fires this frame, returns true and fills outBulletPos / outBulletDir.
    bool update(float dt,
        std::vector<TDEnemy>& enemies,
        sf::Vector2f& outBulletPos,
        sf::Vector2f& outBulletDir);

    // Draw turret
    void render(sf::RenderWindow& window) const;

    const sf::Vector2i& getGrid()  const { return _grid; }
    const sf::RectangleShape& getShape() const { return _shape; }
    TurretType                getType()  const { return _type; }
    const TurretStats& getStats() const { return _stats; }

private:
    sf::Vector2i      _grid;
    sf::RectangleShape _shape;
    float             _tileSize = 0.f;
    float             _cooldown = 0.f;

    TurretType  _type = TurretType::Basic;
    TurretStats _stats{};    // cached stats for this turret
};
