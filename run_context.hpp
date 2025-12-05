#pragma once
#include <memory>
#include "TurretType.hpp"
#include <vector>

// Shared data for a single run of the game (used across scenes)

// Basic player stats that can be tweaked during the run
struct PlayerStats {
    int maxHealth = 5;
    int currentHealth = 5;
    // later: movementSpeed, damage, reloadSpeed, etc.
};

// Global-ish context for the current run (wave, currency, player stats, etc.)
struct RunContext {
    int  waveNumber = 1;   // current wave in the tower defence
    int  currency = 5;   // money earned this run
    bool runOver = false;
    // Inventory of purchased turrets

    PlayerStats playerStats;  // health values for the player

    std::vector<TurretType> turretInventory;

    // --- Level-up upgrades ---


    //turret stats
    float turretDamageMult = 1.0f;  // >1 = turrets deal more damage
    float turretFireRateMult = 1.0f;  // >1 = turrets fire more often
    float turretCostMult = 1.0f;  // <1 = turrets cheaper 
    float turretRangeMult = 1.f;  // Turret Range Up
    float bulletSpeedMult = 1.f;  // Bullet Speed Up
    float explosionRadiusMult = 1.f;  // Splash Radius Up
    float dotDamageMult = 1.f;  // Burn Potency Up
    float slowPercentMult = 1.f;  // Slow Potency Up

    // Economy
    float goldPerKillMult = 1.f;  // Gold Gain Up
    int   waveBonusGold = 0;    // Wave Clear Bonus (+X gold per wave)
    float incomeMult = 1.f;  // Banana Farm Boost

    // Player survivability / stats
    float damageTakenMult = 1.f;  // Armour (1.0 = normal; 0.9 = -10% dmg)
    float playerMoveSpeedMult = 1.f;  // Movement Speed Up
    int   playerMaxHpBonus = 0;    // Max HP Up (bonus on top of base)

    // Shop / free stuff
    int freeShopItemsPerLevel = 0;    // how many free items you earn per LEVEL
    int freeShopItemsPending = 0;    // charges to spend in the shop
    int freeTurretsPerLevel = 0;    // how many free turrets per LEVEL

};

//Making a singleton-like run context
RunContext& get_run_context();
void reset_run_context();