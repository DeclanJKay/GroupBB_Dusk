// TDEnemy.hpp
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "EnemyType.hpp"

class TDEnemy {
public:
    TDEnemy(EnemyType type, const sf::Vector2f& startPos);

    // Move along the shared path and update flash colour.
    // Returns true if this enemy reached the end of the path this frame.
    bool update(float dt, const std::vector<sf::Vector2f>& path, float tileSize);

    // Basic render helper (optional – you can also just use getShape())
    void render(sf::RenderWindow& window) const { window.draw(_shape); }

    // Combat helpers
    void applyDamage(int amount);
    bool isDead() const { return _hp <= 0; }

    // Accessors used by turrets / bullets
    const sf::CircleShape& getShape() const { return _shape; }
    sf::CircleShape& getShape() { return _shape; }
    EnemyType              getType()  const { return _type; }

    // World-space centre position (used by turrets)
    sf::Vector2f getPosition() const { return _shape.getPosition(); }

    // Radius used for simple circle collision
    float getRadius() const { return _shape.getRadius(); }

private:
    EnemyType      _type;
    float          _t = 0.f;          // parametric position along path
    sf::CircleShape _shape;

    int            _hp = 1;
    int            _maxHp = 1;
    float          _speed = 60.f;
    float          _flashTimer = 0.f;
    sf::Color      _baseColor = sf::Color::Red;
};
