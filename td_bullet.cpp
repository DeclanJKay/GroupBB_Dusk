#include "td_bullet.hpp"
#include <cmath>

TDBullet::TDBullet(const sf::Vector2f& startPos,
    const sf::Vector2f& direction,
    float speed,
    int   damage,
    float ttl,
    float explosionRadius,
    float dotDuration,
    float dotDps,
    float slowDuration,
    float slowPercent,
    float stunDuration)
    : _pos(startPos)
    , _vel(direction)
    , _speed(speed)
    , _damage(damage)
    , _ttl(ttl)
    , _explosionRadius(explosionRadius)
    , _dotDuration(dotDuration)
    , _dotDps(dotDps)
    , _slowDuration(slowDuration)
    , _slowPercent(slowPercent)
    , _stunDuration(stunDuration)
{
    // Small white circle while flying
    _shape.setRadius(4.f);
    _shape.setOrigin(4.f, 4.f);
    _shape.setFillColor(sf::Color::White);
    _shape.setPosition(_pos);
}

bool TDBullet::update(float dt, std::vector<TDEnemy>& enemies)
{
    // ---------------------------
    // Explosion phase (visual only)
    // ---------------------------
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

    // ---------------------------
    // Flying bullet phase
    // ---------------------------
    _ttl -= dt;
    if (_ttl <= 0.f) {
        return false; // die quietly
    }

    // Move forwards
    _pos += _vel * _speed * dt;
    _shape.setPosition(_pos);

    // Check if we collided with anything this frame
    bool hitSomething = false;
    for (auto& e : enemies) {
        if (e.isDead()) continue;

        sf::Vector2f enemyPos = e.getPosition();
        float enemyRadius = e.getShape().getRadius();
        float bulletRadius = _shape.getRadius();

        sf::Vector2f d = enemyPos - _pos;
        float distSq = d.x * d.x + d.y * d.y;
        float r = enemyRadius + bulletRadius;

        if (distSq <= r * r) {
            hitSomething = true;
            break;
        }
    }

    if (!hitSomething) {
        // No impact this frame, keep flying
        return true;
    }

    // ---------------------------
    // We hit something: resolve damage (once)
    // ---------------------------
    if (!_hasDealtDamage) {
        if (_explosionRadius <= 0.f) {
            // Single-target bullet
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

                    if (_dotDuration > 0.f && _dotDps > 0.f) {
                        e.applyDot(_dotDuration, _dotDps);
                    }
                    if (_slowDuration > 0.f && _slowPercent > 0.f) {
                        e.applySlow(_slowDuration, _slowPercent);
                    }
                    if (_stunDuration > 0.f) {
                        e.applyStun(_stunDuration);
                    }
                    break;
                }
            }
        }
        else {
            // AoE bullet: affect everything in a blast radius
            for (auto& e : enemies) {
                if (e.isDead()) continue;

                sf::Vector2f enemyPos = e.getPosition();
                float enemyRadius = e.getShape().getRadius();

                float combined = _explosionRadius + enemyRadius;
                float combinedSq = combined * combined;

                sf::Vector2f d = enemyPos - _pos;
                float distSq = d.x * d.x + d.y * d.y;

                if (distSq <= combinedSq) {
                    if (_damage > 0) {
                        e.applyDamage(_damage);
                    }
                    if (_dotDuration > 0.f && _dotDps > 0.f) {
                        e.applyDot(_dotDuration, _dotDps);
                    }
                    if (_slowDuration > 0.f && _slowPercent > 0.f) {
                        e.applySlow(_slowDuration, _slowPercent);
                    }
                    if (_stunDuration > 0.f) {
                        e.applyStun(_stunDuration);
                    }
                }
            }
        }

        _hasDealtDamage = true;
    }

    // ---------------------------
    // Decide if we show an explosion or just vanish
    // ---------------------------
    if (_explosionRadius <= 0.f) {
        // Non-AoE bullets: disappear immediately after dealing damage
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

void TDBullet::startExplosionVisual()
{
    // We only want a visual flash, no damage here.
    _inExplosion = true;
    _hasDealtDamage = true;                    // so update() never tries to damage
    _explosionTimer = _explosionDuration;

    _shape.setRadius(_explosionRadius);
    _shape.setOrigin(_explosionRadius, _explosionRadius);
    _shape.setFillColor(sf::Color(255, 200, 100, 200));
}
