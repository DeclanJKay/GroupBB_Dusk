// TurretStats.hpp
#pragma once

#include <SFML/Graphics/Color.hpp>
#include "TurretType.hpp"

// Base stats per turret type, similar to EnemyStats.
struct TurretStats
{
    float     rangeTiles = 3.f;           // how many tiles it can reach
    float     fireInterval = 0.5f;         // seconds between shots
    sf::Color color = sf::Color::White;

    int       damage = 1;            // direct hit damage
    float     explosionRadius = 0.f;          // AOE radius (world units)
    float     damageOverTime = 0.f;          // DoT per second
    float     dotDuration = 0.f;       // how long the DoT lasts (seconds)
    float     slowDownTime = 0.f;          // how long slow lasts (s)
    float     slowDownPercent = 0.f;          // 0..1 fraction slow
    float     stunTime = 0.f;          // seconds stunned

    bool      isBuff = false;        // buff turret aura
    float     buffDamageMult = 1.0f;         // e.g. 1.25 = +25% damage
    float     buffFireRateMult = 1.0f;         // <1 = faster fire, >1 = slower

    bool      generatesIncome = false;        // money 
    int       incomePerTick = 0;            // how much per tick (per fireInterval)

    int       cost = 0;            // purchase cost
};

// Look up stats for a given turret type
TurretStats get_turret_stats(TurretType type);
