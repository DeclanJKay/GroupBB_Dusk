#pragma once
#include <memory>
//Again doing this here to avoid circular dependencies with Player, Turret, Enemy, etc.


// declare anything you want later (Player, Turret, etc.)
struct PlayerStats {
    int maxHealth = 3;
    int currentHealth = 3;
    // later: movementSpeed, damage, reloadSpeed, etc.
};

struct RunContext {
    int waveNumber = 1;
    int currency = 0;
    bool runOver = false;

    PlayerStats playerStats;

    // later:
    // std::vector<std::shared_ptr<Turret>> turrets;
    // std::vector<std::shared_ptr<Enemy>>  enemies;
};
