#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "TDEnemy.hpp"

// Tower-defence bullet:
//  - Flies in a straight line.
//  - On impact can:
//      * deal single-target damage
//      * OR deal AoE damage in a radius
//      * AND optionally apply a burn DoT.
//  - After hitting, non-AoE bullets vanish,
//    AoE bullets show a brief explosion flash.
class TDBullet {
public:
    TDBullet(const sf::Vector2f& startPos,
        const sf::Vector2f& direction,
        float speed = 300.f,
        int   damage = 1,
        float ttl = 2.0f,
        float explosionRadius = 0.f,   // 0 = no AoE
        float dotDuration = 0.f,   // 0 = no burn
        float dotDps = 0.f);  // damage per second

    // Move the bullet and check for collisions.
    // Returns true if the bullet is still alive after this frame,
    // false if it should be removed.
    bool update(float dt, std::vector<TDEnemy>& enemies);

    // Drawing helper
    void render(sf::RenderWindow& window) const { window.draw(_shape); }

    const sf::CircleShape& getShape() const { return _shape; }

private:
    // Motion / life
    sf::Vector2f   _pos;
    sf::Vector2f   _vel;         // assumed normalised
    float          _speed = 300.f;
    int            _damage = 1;
    float          _ttl = 2.0f;  // time-to-live in seconds

    // Effects
    float          _explosionRadius = 0.f; // AoE radius in world units
    float          _dotDuration = 0.f; // burn duration
    float          _dotDps = 0.f; // burn damage per second

    // Explosion visual state
    bool           _inExplosion = false;
    bool           _hasDealtDamage = false;
    float          _explosionTimer = 0.f;
    float          _explosionDuration = 0.15f;   // how long the flash lasts

    sf::CircleShape _shape;
};
