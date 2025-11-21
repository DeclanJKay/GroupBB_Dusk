// EnemyStats.cpp
#include "EnemyStats.hpp"

EnemyStats get_enemy_stats(EnemyType type) {
    EnemyStats stats{};

    switch (type) {
        // ----------------- CORE / EARLY ENEMIES -----------------
    case EnemyType::Basic:
        stats.hp = 3;
        stats.speed = 60.f;
        stats.radius = 15.f;
        stats.color = sf::Color::Red;
        stats.cost = 1;   // cheap
        break;

    case EnemyType::Fast:
        stats.hp = 2;
        stats.speed = 110.f;
        stats.radius = 12.f;
        stats.color = sf::Color(255, 200, 0); // yellow/orange
        stats.cost = 2;   // pays for speed
        break;

    case EnemyType::Tank:
        stats.hp = 6;
        stats.speed = 40.f;
        stats.radius = 18.f;
        stats.color = sf::Color(150, 0, 200); // purple
        stats.cost = 3;   // chunky
        break;

        // ----------------- LEVEL 2 UNLOCKS -----------------
    case EnemyType::shortRanged:
        stats.hp = 4;
        stats.speed = 70.f;
        stats.radius = 14.f;
        stats.color = sf::Color::Black;
        stats.isRanged = true;
        stats.rangeLimit = 120.f;
        stats.damage = 1;
        stats.cost = 3;
        break;

    case EnemyType::Exploder:
        stats.hp = 2;
        stats.speed = 80.f;
        stats.radius = 14.f;
        stats.color = sf::Color::Blue;
        stats.explodes = true;
        stats.explosionRadius = 80.f;
        stats.damage = 2;
        stats.cost = 3;
        break;

        // ----------------- LEVEL 3 UNLOCKS -----------------
    case EnemyType::Medium:
        stats.hp = 4;
        stats.speed = 75.f;
        stats.radius = 14.f;
        stats.color = sf::Color::Green;
        stats.damage = 2;
        stats.cost = 3;
        break;

    case EnemyType::RangedMelee:
        stats.hp = 5;
        stats.speed = 70.f;
        stats.radius = 15.f;
        stats.color = sf::Color::Magenta;
        stats.isRanged = true;
        stats.rangeLimit = 150.f;
        stats.damage = 2;
        stats.cost = 4;
        break;

    case EnemyType::FastExploder:
        stats.hp = 2;
        stats.speed = 120.f;
        stats.radius = 13.f;
        stats.color = sf::Color::Cyan;
        stats.explodes = true;
        stats.explosionRadius = 90.f;
        stats.damage = 3;
        stats.cost = 4;
        break;

        // ----------------- LEVEL 4 UNLOCKS -----------------
    case EnemyType::LongRange:
        stats.hp = 3;
        stats.speed = 65.f;
        stats.radius = 13.f;
        stats.color = sf::Color(100, 200, 255);
        stats.isRanged = true;
        stats.rangeLimit = 220.f;
        stats.damage = 2;
        stats.cost = 4;
        break;

    case EnemyType::HeavyTank:
        stats.hp = 10;
        stats.speed = 35.f;
        stats.radius = 20.f;
        stats.color = sf::Color(80, 80, 80);
        stats.damage = 3;
        stats.cost = 5;
        break;

        // ----------------- BOSSES (ONE PER LEVEL) -----------------
    case EnemyType::Boss1:
        stats.hp = 30;
        stats.speed = 55.f;
        stats.radius = 24.f;
        stats.color = sf::Color(255, 100, 100);
        stats.damage = 4;
        stats.cost = 999; // we don’t use cost for bosses
        break;

    case EnemyType::Boss2:
        stats.hp = 40;
        stats.speed = 60.f;
        stats.radius = 26.f;
        stats.color = sf::Color(255, 160, 80);
        stats.damage = 5;
        stats.cost = 999;
        break;

    case EnemyType::Boss3:
        stats.hp = 50;
        stats.speed = 65.f;
        stats.radius = 28.f;
        stats.color = sf::Color(255, 220, 80);
        stats.damage = 6;
        stats.cost = 999;
        break;

    case EnemyType::Boss4:
        stats.hp = 65;
        stats.speed = 70.f;
        stats.radius = 30.f;
        stats.color = sf::Color(200, 120, 255);
        stats.damage = 7;
        stats.cost = 999;
        break;

    case EnemyType::Boss5:
        stats.hp = 80;
        stats.speed = 75.f;
        stats.radius = 32.f;
        stats.color = sf::Color(255, 255, 255);
        stats.damage = 8;
        stats.cost = 999;
        break;
    }

    return stats;
}
