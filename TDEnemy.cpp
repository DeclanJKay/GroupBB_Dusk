// TDEnemy.cpp
#include "TDEnemy.hpp"
#include "EnemyStats.hpp"    // for get_enemy_stats

#include <algorithm>         // std::max

TDEnemy::TDEnemy(EnemyType type, const sf::Vector2f& startPos)
    : _type(type)
{
    // Pull shared stats (HP / speed / radius / colour) from the central table
    EnemyStats stats = get_enemy_stats(type);

    _hp = stats.hp;
    _maxHp = stats.hp;
    _speed = stats.speed;
    _baseColor = stats.color;

    _shape.setRadius(stats.radius);
    _shape.setOrigin(stats.radius, stats.radius);
    _shape.setFillColor(_baseColor);
    _shape.setPosition(startPos);
}

bool TDEnemy::update(float dt,
                     const std::vector<sf::Vector2f>& path,
                     float tileSize)
{

    // --- Burn / damage-over-time tick ---
    if (_dotTimeRemaining > 0.f && _dotDps > 0.f) {
        _dotTimeRemaining -= dt;
        if (_dotTimeRemaining < 0.f) _dotTimeRemaining = 0.f;

        // accumulate fractional damage over time
        _dotAccumulator += _dotDps * dt;
        while (_dotAccumulator >= 1.f && _hp > 0) {
            _hp -= 1;
            _dotAccumulator -= 1.f;

            // optional: small flash when burn ticks
            _flashTimer = 0.2f;
        }

        // If burn killed the enemy, we stop here.
        // TowerDefenceScene removes it because isDead() will be true.
        if (_hp <= 0) {
            return false;
        }
    }
	// --- Movement along path ---
    if (path.size() < 2) {
        return false; // nowhere to go
    }

    // Move along the path in "tile segments"
    float deltaT = (_speed * dt) / tileSize;
    _t += deltaT;

    int   segment = static_cast<int>(_t);
    float local = _t - segment;

    if (segment >= static_cast<int>(path.size()) - 1) {
        // Clamp to final node
        _shape.setPosition(path.back());
    }
    else {
        const sf::Vector2f& a = path[static_cast<size_t>(segment)];
        const sf::Vector2f& b = path[static_cast<size_t>(segment + 1)];
        _shape.setPosition(a + (b - a) * local);
    }

    // Hit flash colour
    if (_flashTimer > 0.f) {
        _flashTimer -= dt;
        float t = std::max(_flashTimer / 0.2f, 0.f);

        sf::Color c;
        c.r = static_cast<sf::Uint8>(255);
        c.g = static_cast<sf::Uint8>(180 + 75 * t);
        c.b = static_cast<sf::Uint8>(180 + 75 * t);
        c.a = 255;
        _shape.setFillColor(c);
    }
    else {
        _shape.setFillColor(_baseColor);
    }

    // Let the scene decide what to do if it’s at the end
    return (segment >= static_cast<int>(path.size()) - 1);
}

void TDEnemy::applyDamage(int amount)
{
    if (_hp <= 0) return;

    _hp -= amount;
    if (_hp < 0) _hp = 0;

    _flashTimer = 0.2f;  // trigger short flash
}

void TDEnemy::applyDot(float duration, float dps)
{
    if (duration <= 0.f || dps <= 0.f) return;

    // For now we just overwrite any existing burn with the new one.
    _dotTimeRemaining = duration;
    _dotDps = dps;
    // we are keeping _dotAccumulator as-is so fractional damage carries on smoothly
}

