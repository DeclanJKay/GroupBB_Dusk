#pragma once

#include <map>
#include <unordered_map>
#include "Comps.hpp"
#include <SFML/Graphics.hpp>

//struct with all the variables necessary to spawn an enemy
struct EnemyStats
{
    std::shared_ptr<sf::Texture> txtr; //replace with sprite when implemented
    int hp;
    int speed;
    float moveShootDelay;
    int friction;
    int radius;
    WeaponArsenal weapons; //THE RADIUS AND BULLET RADIUS GETS ADDED TO OFFSET.Y BY DEFAULT DURING CREATION (spawns in front of enemy)
    std::vector<int> ranges;
    int shieldAmount = 0;
    sf::Vector2i swapCDrange = {2,5};
};

//class for defining all stats for each enemy type
//MAKE IT A SINGLETON?
class EnemyStatsManager
{
    //prevent construction
    EnemyStatsManager() = delete;
    ~EnemyStatsManager() = delete;

    private:
        const static std::unordered_map<EnemyTypes, int> EnemyToCost;
        const static std::vector<EnemyTypes> enemiesPerLevel[]; 

    public:
        static EnemyStats GetStats(EnemyTypes type);
        static int GetCost(EnemyTypes type);
        static std::map<int, std::vector<EnemyTypes>> GetLevelCostMap(int levelInd);
        static EnemyTypes GetBoss();
};