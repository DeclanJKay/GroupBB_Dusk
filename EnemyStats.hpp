#pragma once
#include "Systems.hpp"
#include <map>

//struct with all the variables necessary to spawn an enemy
struct EnemyStats
{
    sf::Color col; //replace with sprite when implemented
    int hp;
    int speed;
    float moveShootDelay;
    int friction;
    int radius;
    WeaponArsenal weapons;
};

//class for defining all stats for each enemy type
class EnemyStatsManager
{
    //enemy types per level, sorted and grouped by cost
    const static std::map<int, std::vector<EnemyTypes>> enemiesPerLevel[]; 
    EnemyStatsManager() = delete;
    ~EnemyStatsManager() = delete;
    public:
        static EnemyStats GetStats(EnemyTypes type);
};