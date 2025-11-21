// EnemyStats.cpp
#include "EnemyStats.hpp"

EnemyStats get_enemy_stats(EnemyType type) {
    EnemyStats stats; // starts with safe defaults

    switch (type) {
    case EnemyType::Basic:
        stats.hp = 3;
        stats.speed = 60.f;
        stats.radius = 15.f;
        stats.color = sf::Color::Red;
        stats.damage = 1;
        break;

    case EnemyType::Fast:
        stats.hp = 2;
        stats.speed = 110.f;
        stats.radius = 12.f;
        stats.color = sf::Color(255, 200, 0); // yellow/orange
        stats.damage = 1;
        break;

    case EnemyType::Tank:
        stats.hp = 8;
        stats.speed = 40.f;
        stats.radius = 18.f;
        stats.color = sf::Color(150, 0, 200); // purple
        stats.damage = 2;
        break;

    case EnemyType::shortRanged:
        stats.hp = 4;
        stats.speed = 70.f;
        stats.radius = 14.f;
        stats.color = sf::Color::Black;
        stats.isRanged = true;
        stats.rangeLimit = 100.f;
        stats.damage = 1;
        break;

    case EnemyType::Exploder:
        stats.hp = 3;
        stats.speed = 50.f;
        stats.radius = 14.f;
        stats.color = sf::Color::Blue;
        stats.explodes = true;
        stats.explosionRadius = 80.f;
        stats.damage = 3;
        break;

    case EnemyType::Medium:
        stats.hp = 5;
        stats.speed = 70.f;
        stats.radius = 15.f;
        stats.color = sf::Color::Green;
        stats.damage = 1;
        break;

    case EnemyType::RangedMelee:
        stats.hp = 6;
        stats.speed = 65.f;
        stats.radius = 15.f;
        stats.color = sf::Color::Magenta;
        stats.isRanged = true;
        stats.rangeLimit = 140.f;
        stats.damage = 2;
        break;

    case EnemyType::FastExploder:
        stats.hp = 3;
        stats.speed = 120.f;
        stats.radius = 12.f;
        stats.color = sf::Color::Cyan;
        stats.explodes = true;
        stats.explosionRadius = 70.f;
        stats.damage = 3;
        break;

    case EnemyType::LongRange:
        stats.hp = 4;
        stats.speed = 55.f;
        stats.radius = 14.f;
        stats.color = sf::Color(100, 200, 255);
        stats.isRanged = true;
        stats.rangeLimit = 200.f;
        stats.damage = 1;
        break;

    case EnemyType::HeavyTank:
        stats.hp = 15;
        stats.speed = 30.f;
        stats.radius = 20.f;
        stats.color = sf::Color(80, 80, 80);
        stats.damage = 3;
        break;

    case EnemyType::Boss1:
    case EnemyType::Boss2:
    case EnemyType::Boss3:
    case EnemyType::Boss4:
    case EnemyType::Boss5:
        stats.hp = 40;
        stats.speed = 45.f;
        stats.radius = 22.f;
        stats.color = sf::Color(255, 50, 150);
        stats.damage = 4;
		// we can differentiate bosses by type later on 
        break;
    }

    return stats;
}
