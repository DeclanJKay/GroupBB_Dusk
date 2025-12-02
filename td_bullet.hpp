#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "TDEnemy.hpp"

// Simple tower-defence bullet: straight-line, optional AoE + DoT + slow.
class TDBullet {
public:
    TDBullet(const sf::Vector2f& startPos,
        const sf::Vector2f& direction,
        float speed = 300.f,
        int   damage = 1,
        float ttl = 2.0f,
        float explosionRadius = 0.f,
        float dotDuration = 0.f,
        float dotDps = 0.f,
        float slowDuration = 0.f,
        float slowPercent = 0.f);

    // Move the bullet and check for collisions.
    // Returns true if the bullet is still alive after this frame,
    // false if it should be removed.
    bool update(float dt, std::vector<TDEnemy>& enemies);

    // Drawing helper
    void render(sf::RenderWindow& window) const { window.draw(_shape); }

    const sf::CircleShape& getShape() const { return _shape; }

private:
    sf::Vector2f   _pos;
    sf::Vector2f   _vel;               // assumed normalised
    float          _speed;
    int            _damage;
    float          _ttl;               // time-to-live in seconds

    float          _explosionRadius;   // AoE radius in world units (0 = no AoE)
    float          _dotDuration;       // burn duration
    float          _dotDps;            // burn DPS
    float          _slowDuration;      // slow duration
    float          _slowPercent;       // 0..1 fraction slow

    // Explosion visual state
    bool           _inExplosion = false;
    bool           _hasDealtDamage = false;
    float          _explosionTimer = 0.f;
    float          _explosionDuration = 0.15f;   // how long the flash lasts

    sf::CircleShape _shape;
};
