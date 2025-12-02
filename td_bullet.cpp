#include "td_bullet.hpp"
#include <cmath>

TDBullet::TDBullet(const sf::Vector2f& startPos,
    const sf::Vector2f& direction,
    float speed,
    int   damage,
    float ttl,
    float explosionRadius)
    : _pos(startPos)
    , _vel(direction)
    , _speed(speed)
    , _damage(damage)
    , _ttl(ttl)
    , _explosionRadius(explosionRadius)
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

    // First: check if we hit *anything* with the bullet's small radius
    bool hit = false;
    for (auto& e : enemies) {
        if (e.isDead()) continue;

        sf::Vector2f enemyPos = e.getPosition();
        float enemyRadius = e.getShape().getRadius();
        float bulletRadius = _shape.getRadius();

        sf::Vector2f d = enemyPos - _pos;
        float distSq = d.x * d.x + d.y * d.y;
        float r = enemyRadius + bulletRadius;

        if (distSq <= r * r) {
            hit = true;
            break;
        }
    }

    if (!hit) {
        // No impact this frame, keep flying
        return true;
    }

    // --- We hit something: resolve damage ---

    if (_explosionRadius <= 0.f) {
        // Single-target bullet: damage the first enemy we collide with
        for (auto& e : enemies) {
            if (e.isDead()) continue;

            sf::Vector2f enemyPos = e.getPosition();
            float enemyRadius = e.getShape().getRadius();
            float bulletRadius = _shape.getRadius();

            sf::Vector2f d = enemyPos - _pos;
            float distSq = d.x * d.x + d.y * d.y;
            float r = enemyRadius + bulletRadius;

            if (distSq <= r * r) {
                e.applyDamage(_damage);
                break;
            }
        }
    }
    else {
        // AoE bullet: damage everything in a blast radius around impact point
        for (auto& e : enemies) {
            if (e.isDead()) continue;

            sf::Vector2f enemyPos = e.getPosition();
            float enemyRadius = e.getShape().getRadius();

            float combined = _explosionRadius + enemyRadius;
            float combinedSq = combined * combined;

            sf::Vector2f d = enemyPos - _pos;
            float distSq = d.x * d.x + d.y * d.y;

            if (distSq <= combinedSq) {
                e.applyDamage(_damage);
            }
        }
    }

    // Bullet is consumed on impact
    return false;
}
