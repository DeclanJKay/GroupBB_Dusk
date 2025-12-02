// TurretStats.cpp
#include "TurretStats.hpp"

TurretStats get_turret_stats(TurretType type)
{
    TurretStats stats{}; // gets all the defaults from the struct

    switch (type)
    {
        // -------- Basic --------
    case TurretType::Basic:
        stats.rangeTiles = 4.f;
        stats.fireInterval = 0.8f;
        stats.damage = 1;
        stats.explosionRadius = 0.f;
        stats.damageOverTime = 0.f;
        stats.slowDownTime = 0.f;
        stats.slowDownPercent = 0.f;
        stats.stunTime = 0.f;
        stats.isBuff = false;
        stats.generatesIncome = false;
        stats.color = sf::Color::Cyan;
        stats.cost = 4;

        stats.bulletSpeed = 300.f;
        stats.bulletTtl = 2.0f;
        break;

        // -------- SMG (short range, fast fire) --------
    case TurretType::SMG:
        stats.rangeTiles = 2.f;
        stats.fireInterval = 0.4f;
        stats.damage = 1;
        stats.color = sf::Color::Blue;
        stats.cost = 6;

        stats.bulletSpeed = 320.f;
        stats.bulletTtl = 1.5f;
        break;

        // -------- Sniper (long range, slow fire, high damage) --------
    case TurretType::Sniper:
        stats.rangeTiles = 6.f;
        stats.fireInterval = 1.2f;
        stats.damage = 4;
        stats.color = sf::Color::Red;
        stats.cost = 10;

        stats.bulletSpeed = 420.f;   // faster, flies further
        stats.bulletTtl = 3.0f;    // lives longer
        break;

        // -------- Bomb (AOE damage) --------
    case TurretType::Bomb:
        stats.rangeTiles = 3.f;
        stats.fireInterval = 0.9f;
        stats.damage = 2;
        stats.explosionRadius = 80.f;
        stats.color = sf::Color(200, 150, 0);
        stats.cost = 8;

        stats.bulletSpeed = 260.f;   // slower chunky shells
        stats.bulletTtl = 2.5f;
        break;

        // -------- Fire (DoT) --------
    case TurretType::Fire:
        stats.rangeTiles = 3.f;
        stats.fireInterval = 0.8f;
        stats.damage = 0;
        stats.damageOverTime = 1.f;    // DPS
        stats.dotDuration = 3.0f;    // DoT lasts 3 seconds
        stats.color = sf::Color(255, 120, 0);
        stats.cost = 7;

        stats.bulletSpeed = 280.f;
        stats.bulletTtl = 2.0f;
        break;

        // -------- Lightening (stun) --------
    case TurretType::Lightening:
        stats.rangeTiles = 3.f;
        stats.fireInterval = 0.8f;
        stats.damage = 1;
        stats.stunTime = 0.5f;
        stats.color = sf::Color(180, 180, 255);
        stats.cost = 7;

        stats.bulletSpeed = 340.f;
        stats.bulletTtl = 2.0f;
        break;

        // -------- Freeze (slow + damage) --------
    case TurretType::Freeze:
        stats.rangeTiles = 3.f;
        stats.fireInterval = 0.8f;
        stats.damage = 1;
        stats.slowDownTime = 1.5f;
        stats.slowDownPercent = 0.5f;     // 50% slow
        stats.color = sf::Color(150, 220, 255);
        stats.cost = 7;

        stats.bulletSpeed = 280.f;
        stats.bulletTtl = 2.0f;
        break;

        // -------- Buff (buff nearby turrets) --------
    case TurretType::Buff:
        stats.rangeTiles = 3.f;
        stats.fireInterval = 1.0f;
        stats.damage = 0;
        stats.isBuff = true;
        stats.buffDamageMult = 1.25f;
        stats.buffFireRateMult = 0.8f;
        stats.color = sf::Color(200, 255, 200);
        stats.cost = 10;

        stats.bulletSpeed = 300.f;
        stats.bulletTtl = 2.0f;
        break;

        // -------- Scatter (shotgun style) --------
    case TurretType::Scatter:
        stats.rangeTiles = 3.f;
        stats.fireInterval = 0.7f;
        stats.damage = 1;
        stats.color = sf::Color(255, 255, 0);
        stats.cost = 9;

        stats.bulletSpeed = 300.f;
        stats.bulletTtl = 1.8f;
        break;

        // -------- AOE aura around turret --------
    case TurretType::AOE:
        stats.rangeTiles = 2.f;
        stats.fireInterval = 0.6f;
        stats.damage = 1;
        stats.explosionRadius = 60.f;
        stats.color = sf::Color(255, 180, 0);
        stats.cost = 8;

        stats.bulletSpeed = 260.f;
        stats.bulletTtl = 2.0f;
        break;

        // -------- Slow aura / no damage --------
    case TurretType::Slow:
        stats.rangeTiles = 3.f;
        stats.fireInterval = 1.0f;
        stats.damage = 0;
        stats.slowDownTime = 1.5f;
        stats.slowDownPercent = 0.6f;
        stats.color = sf::Color(120, 220, 220);
        stats.cost = 6;

        stats.bulletSpeed = 260.f;
        stats.bulletTtl = 2.0f;
        break;

        // -------- BananaFarm / income generator --------
    case TurretType::BananaFarm:
        stats.rangeTiles = 0.f;
        stats.fireInterval = 2.0f;
        stats.damage = 0;
        stats.generatesIncome = true;
        stats.incomePerTick = 5;
        stats.color = sf::Color(255, 230, 120);
        stats.cost = 15;

        stats.bulletSpeed = 0.f;
        stats.bulletTtl = 0.f;
        break;
    }

    return stats;
}
