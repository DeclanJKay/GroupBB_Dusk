// EnemyStats.hpp
#pragma once

#include <SFML/Graphics/Color.hpp>
#include "EnemyType.hpp"

// Simple container for base stats per enemy type
struct EnemyStats {
    int   hp;
    float speed;
    float radius;
    sf::Color color;
};

// Get the stats for a given EnemyType
EnemyStats get_enemy_stats(EnemyType type);
