#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "TDEnemy.hpp"

// Simple tower-defence bullet: flies in a straight line, damages the
// first enemy it hits, or disappears when its lifetime runs out.
class TDBullet {
public:
    TDBullet(const sf::Vector2f& startPos,
        const sf::Vector2f& direction,
        float speed = 300.f,
        int   damage = 1,
        float ttl = 2.0f);

    // Move the bullet and check for collisions.
    // Returns true if the bullet is still alive after this frame,
    // false if it should be removed.
    bool update(float dt, std::vector<TDEnemy>& enemies);

    // Drawing helper
    void render(sf::RenderWindow& window) const { window.draw(_shape); }

    const sf::CircleShape& getShape() const { return _shape; }

private:
    sf::Vector2f   _pos;
    sf::Vector2f   _vel;      // assumed normalised
    float          _speed;
    int            _damage;
    float          _ttl;      // time-to-live in seconds
    sf::CircleShape _shape;
};
