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
    EnemyType               getType()  const { return _type; }

    // World-space centre position (used by turrets)
    sf::Vector2f getPosition() const { return _shape.getPosition(); }

    // Radius used for simple circle collision
    float getRadius() const { return _shape.getRadius(); }

    // Apply status effects
    void applyDot(float duration, float dps);
    void applySlow(float duration, float percent);
    void applyStun(float duration);

private:
    EnemyType      _type;
    float          _t = 0.f;          // parametric position along path
    sf::CircleShape _shape;

    int            _hp = 1;
    int            _maxHp = 1;
    float          _speed = 60.f;
    float          _flashTimer = 0.f;
    sf::Color      _baseColor = sf::Color::Red;

    // --- Burn / DoT state ---
    float _dotTimeRemaining = 0.f;   // how long the burn lasts (seconds)
    float _dotDps = 0.f;   // damage per second
    float _dotAccumulator = 0.f;   // accumulates fractional damage

    // --- Slow state ---
    float _slowTimeRemaining = 0.f;   // how long the slow lasts
    float _slowPercent = 0.f;   // 0..1 fraction (0.5 = 50% slower)

    // --- Stun state ---
    float _stunTimeRemaining = 0.f;   // how long the enemy is stunned
};
