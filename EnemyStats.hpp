// EnemyStats.hpp
#pragma once

#include <SFML/Graphics/Color.hpp>
#include "EnemyType.hpp"

// Base stats per enemy type
struct EnemyStats {
    int   hp = 1;
    float speed = 60.f;
    float radius = 12.f;
    sf::Color color = sf::Color::White;

    bool  isRanged = false;
    float rangeLimit = 0.f;   // how far a ranged enemy can shoot
    int   damage = 1;         // contact or projectile damage
    bool  explodes = false;
    float explosionRadius = 0.f;
};

// Get the stats for a given EnemyType
EnemyStats get_enemy_stats(EnemyType type);
