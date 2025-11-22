// EnemyStats.hpp
#pragma once

#include <SFML/Graphics/Color.hpp>
#include "EnemyType.hpp"

// Base stats per enemy type, shared between TD and Safehouse
struct EnemyStats {
    int   hp = 1;
    float speed = 60.f;
    float radius = 12.f;
    sf::Color color = sf::Color::White;

    // Hooks for later behaviours (not fully used yet)
    bool  isRanged = false;
    float rangeLimit = 0.f;
    int   damage = 1;
    bool  explodes = false;
    float explosionRadius = 0.f;

    //point cost used by wave generator
    int   cost;
};

// Get the stats for a given EnemyType
EnemyStats get_enemy_stats(EnemyType type);
