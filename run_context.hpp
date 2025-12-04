#pragma once
#include <memory>
#include "TurretType.hpp"
#include <vector>

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
    // Inventory of purchased turrets

    PlayerStats playerStats;  // health values for the player

    std::vector<TurretType> turretInventory;

    // --- Level-up upgrades ---
    float turretDamageMult = 1.0f;  // >1 = turrets deal more damage
    float turretFireRateMult = 1.0f;  // >1 = turrets fire more often
    float turretCostMult = 1.0f;  // <1 = turrets cheaper 

    // later:
    // std::vector<std::shared_ptr<Turret>> turrets;
    // std::vector<std::shared_ptr<Enemy>>  enemies;
};

//Making a singleton-like run context
RunContext& get_run_context();
void reset_run_context();