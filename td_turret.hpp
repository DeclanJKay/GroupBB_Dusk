#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class TDEnemy;

// Turret that lives on the TD grid and shoots at enemies
class TDTurret {
public:
    // grid = tile coordinates, worldPos = top-left of tile
    TDTurret(const sf::Vector2i& grid, const sf::Vector2f& worldPos, float tileSize);

    // Update cooldown and target enemies.
    // If it fires this frame, returns true and fills outBulletPos / outBulletDir.
    bool update(float dt,
        std::vector<TDEnemy>& enemies,
        sf::Vector2f& outBulletPos,
        sf::Vector2f& outBulletDir);

    // Draw turret
    void render(sf::RenderWindow& window) const;

    const sf::Vector2i& getGrid() const { return _grid; }
    const sf::RectangleShape& getShape() const { return _shape; }

private:
    sf::Vector2i      _grid;
    sf::RectangleShape _shape;
    float             _tileSize;
    float             _cooldown = 0.f;

    static constexpr float kRangeTiles = 3.f;
    static constexpr float kFireInterval = 0.5f;
};
