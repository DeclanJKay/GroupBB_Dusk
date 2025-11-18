#pragma once
#include <memory>

// Shared data for a single run of the game (used across scenes)

// Basic player stats that can be tweaked during the run
struct PlayerStats {
    int maxHealth = 3;
    int currentHealth = 3;
    // later: movementSpeed, damage, reloadSpeed, etc.
};

// Global-ish context for the current run (wave, currency, player stats, etc.)
struct RunContext {
    int  waveNumber = 1;   // current wave in the tower defence
    int  currency = 0;   // money earned this run
    bool runOver = false;

    PlayerStats playerStats;  // health values for the player

    // later:
    // std::vector<std::shared_ptr<Turret>> turrets;
    // std::vector<std::shared_ptr<Enemy>>  enemies;
};
