#include "td_bullet.hpp"
#include <cmath>

TDBullet::TDBullet(const sf::Vector2f& startPos,
    const sf::Vector2f& direction,
    float speed,
    int   damage,
    float ttl)
    : _pos(startPos),
    _vel(direction),
    _speed(speed),
    _damage(damage),
    _ttl(ttl)
{
    // Small white circle
    _shape.setRadius(4.f);
    _shape.setOrigin(4.f, 4.f);
    _shape.setFillColor(sf::Color::White);
    _shape.setPosition(_pos);
}

bool TDBullet::update(float dt, std::vector<TDEnemy>& enemies) {
    // Lifetime countdown
    _ttl -= dt;
    if (_ttl <= 0.f) {
        return false; // die quietly
    }

    // Move forwards
    _pos += _vel * _speed * dt;
    _shape.setPosition(_pos);

    // Check collision against enemies
    for (auto& e : enemies) {
        if (e.isDead()) continue;

        sf::Vector2f enemyPos = e.getPosition();
        float enemyRadius = e.getShape().getRadius();
        float bulletRadius = _shape.getRadius();

        sf::Vector2f d = enemyPos - _pos;
        float distSq = d.x * d.x + d.y * d.y;
        float r = enemyRadius + bulletRadius;

        if (distSq <= r * r) {
            // Hit: apply damage, enemy handles its own flash
            e.applyDamage(_damage);
            return false; // bullet consumed
        }
    }

    // Still flying
    return true;
}
