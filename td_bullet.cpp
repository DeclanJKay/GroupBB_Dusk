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
    // Small white circle while flying
    _shape.setRadius(4.f);
    _shape.setOrigin(4.f, 4.f);
    _shape.setFillColor(sf::Color::White);
    _shape.setPosition(_pos);
}

bool TDBullet::update(float dt, std::vector<TDEnemy>& enemies) {

    if (_inExplosion) {
        _explosionTimer -= dt;
        if (_explosionTimer <= 0.f) {
            return false; // explosion finished, remove bullet
        }

        // Fade out over time
        float t = _explosionTimer / _explosionDuration;
        if (t < 0.f) t = 0.f;
        if (t > 1.f) t = 1.f;

        sf::Color c = _shape.getFillColor();
        c.a = static_cast<sf::Uint8>(255 * t);
        _shape.setFillColor(c);

        // Stay positioned where we detonated
        _shape.setPosition(_pos);
        return true;
    }

    // Lifetime countdown for flying bullet
    _ttl -= dt;
    if (_ttl <= 0.f) {
        return false; // die quietly
    }

    // Move forwards
    _pos += _vel * _speed * dt;
    _shape.setPosition(_pos);

    // Check collision against enemies (did we hit anything?)
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

    // --- We hit something: resolve damage (once) ---
    if (!_hasDealtDamage) {
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

        _hasDealtDamage = true;
    }

    // --- Decide if we show an explosion or just vanish ---
    if (_explosionRadius <= 0.f) {
        // Non-AoE bullets: disappear immediately (same behaviour as before)
        return false;
    }

    // AoE bullets: start explosion flash
    _inExplosion = true;
    _explosionTimer = _explosionDuration;

    // Make the shape big and coloured for the explosion
    _shape.setRadius(_explosionRadius);
    _shape.setOrigin(_explosionRadius, _explosionRadius);
    _shape.setFillColor(sf::Color(255, 200, 100, 200));
    _shape.setPosition(_pos);

    // Keep this "bullet" around as a visual effect until explosionTimer runs out
    return true;
}
