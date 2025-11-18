// EnemyStats.cpp
#include "EnemyStats.hpp"

// Central place to define HP/speed/radius/colour per type
EnemyStats get_enemy_stats(EnemyType type) {
    EnemyStats stats{};

    switch (type) {
    case EnemyType::Basic:
        stats.hp = 3;
        stats.speed = 60.f;
        stats.radius = 15.f;
        stats.color = sf::Color::Red;
        break;

    case EnemyType::Fast:
        stats.hp = 2;
        stats.speed = 110.f;
        stats.radius = 12.f;
        stats.color = sf::Color(255, 200, 0); // yellow/orange
        break;

    case EnemyType::Tank:
        stats.hp = 6;
        stats.speed = 40.f;
        stats.radius = 18.f;
        stats.color = sf::Color(150, 0, 200); // purple
        break;
    }

    return stats;
}
