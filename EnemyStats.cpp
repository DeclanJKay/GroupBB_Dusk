#include "EnemyStats.hpp"
#include "Comps.hpp"

const std::unordered_map<EnemyTypes, int> EnemyStatsManager::EnemyToCost
{
    {EnemyTypes::Basic, 1},
    {EnemyTypes::Fast, 2},
    {EnemyTypes::Tank, 3},
    {EnemyTypes::shortRanged, 3},
    {EnemyTypes::Exploder, 3},
    {EnemyTypes::Medium, 3},
    {EnemyTypes::RangedMelee, 4},
    {EnemyTypes::FastExploder, 4},
    {EnemyTypes::LongRange, 4},
    {EnemyTypes::HeavyTank, 5}
};

const std::vector<EnemyTypes> EnemyStatsManager::enemiesPerLevel[]
{
    //level 1
    {
        EnemyTypes::Basic,
        EnemyTypes::Fast,
        EnemyTypes::Tank
    },

    //level 2
    {
        EnemyTypes::shortRanged, 
        EnemyTypes::Exploder
    },

    //level 3
    {
        EnemyTypes::Medium,
        EnemyTypes::RangedMelee, 
        EnemyTypes::FastExploder
    },

    //level 4
    {
        EnemyTypes::LongRange,
        EnemyTypes::HeavyTank
    }
};

//assign stats based on enemy type given, and returns the struct
EnemyStats EnemyStatsManager::GetStats(EnemyTypes type)
{
    //to add weapons, range 
    EnemyStats stats;
    switch (type)
    {
        case EnemyTypes::Basic:
            //basic stats
            stats.hp = 3;
            stats.speed = 300;//60.f;
            stats.radius = 15.f;
            stats.col = sf::Color::Red;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 2; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 100;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].bulletLifetime = 3;
            stats.weapons.weapons[0].damage = 1;
            stats.weapons.weapons[0].bulletRadius = 10;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
        break;

        case EnemyTypes::Fast:
            stats.hp = 2;
            stats.speed = 110.f;
            stats.radius = 12.f;
            stats.col = sf::Color(255, 200, 0); // yellow/orange
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;
        break;

        case EnemyTypes::Tank:
            stats.hp = 6;
            stats.speed = 40.f;
            stats.radius = 18.f;
            stats.col = sf::Color(150, 0, 200); // purple
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;
            break;

            // ----------------- LEVEL 2 UNLOCKS -----------------
        case EnemyTypes::shortRanged:
            stats.hp = 4;
            stats.speed = 70.f;
            stats.radius = 14.f;
            stats.col = sf::Color::Black;
            //stats.isRanged = true;
            //stats.rangeLimit = 120.f;
            //stats.damage = 1;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;
            break;

        case EnemyTypes::Exploder:
            stats.hp = 2;
            stats.speed = 80.f;
            stats.radius = 14.f;
            stats.col = sf::Color::Blue;
            //stats.explodes = true;
            //stats.explosionRadius = 80.f;
            //stats.damage = 2;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;
            break;

            // ----------------- LEVEL 3 UNLOCKS -----------------
        case EnemyTypes::Medium:
            stats.hp = 4;
            stats.speed = 75.f;
            stats.radius = 14.f;
            stats.col = sf::Color::Green;
            //stats.damage = 2;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;
            break;

        case EnemyTypes::RangedMelee:
            stats.hp = 5;
            stats.speed = 70.f;
            stats.radius = 15.f;
            stats.col = sf::Color::Magenta;
            //stats.isRanged = true;
            //stats.rangeLimit = 150.f;
            //stats.damage = 2;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;
            break;

        case EnemyTypes::FastExploder:
            stats.hp = 2;
            stats.speed = 120.f;
            stats.radius = 13.f;
            stats.col = sf::Color::Cyan;
            //stats.explodes = true;
            //stats.explosionRadius = 90.f;
            //stats.damage = 3;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;
            break;

            // ----------------- LEVEL 4 UNLOCKS -----------------
        case EnemyTypes::LongRange:
            stats.hp = 3;
            stats.speed = 65.f;
            stats.radius = 13.f;
            stats.col = sf::Color(100, 200, 255);
            //stats.isRanged = true;
            //stats.rangeLimit = 220.f;
            //stats.damage = 2;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;
            break;

        case EnemyTypes::HeavyTank:
            stats.hp = 10;
            stats.speed = 35.f;
            stats.radius = 20.f;
            stats.col = sf::Color(80, 80, 80);
            //stats.damage = 3;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;
            break;

            // ----------------- BOSSES (ONE PER LEVEL) -----------------
        case EnemyTypes::Boss1:
            stats.hp = 30;
            stats.speed = 55.f;
            stats.radius = 24.f;
            stats.col = sf::Color(255, 100, 100);
            //stats.damage = 4;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;
            break;

        case EnemyTypes::Boss2:
            stats.hp = 40;
            stats.speed = 60.f;
            stats.radius = 26.f;
            stats.col = sf::Color(255, 160, 80);
            //stats.damage = 5;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;
            break;

        case EnemyTypes::Boss3:
            stats.hp = 50;
            stats.speed = 65.f;
            stats.radius = 28.f;
            stats.col = sf::Color(255, 220, 80);
            //stats.damage = 6;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;
            break;

        case EnemyTypes::Boss4:
            stats.hp = 65;
            stats.speed = 70.f;
            stats.radius = 30.f;
            stats.col = sf::Color(200, 120, 255);
            //stats.damage = 7;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;
            break;

        case EnemyTypes::Boss5:
            stats.hp = 80;
            stats.speed = 75.f;
            stats.radius = 32.f;
            stats.col = sf::Color(255, 255, 255);
            //stats.damage = 8;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;
            break;
    }
    return stats;
}

int EnemyStatsManager::GetCost(EnemyTypes type)
{
    if (EnemyToCost.find(type) == EnemyToCost.end()) {return -1;} //returns -1 if invalid
    return EnemyToCost.at(type);
}

std::map<int, std::vector<EnemyTypes>> EnemyStatsManager::GetLevelCostMap(int levelInd)
{
    std::map<int, std::vector<EnemyTypes>> returnable;
    for (int i = 0; i <= levelInd; i++)
    {
        for (auto type : enemiesPerLevel[i])
        {
            auto cost = GetCost(type);
            if (returnable.find(cost) == returnable.end())
            {
                returnable.insert({cost, {type}});
                continue;
            }
            returnable.at(cost).push_back(type);
        }
    }
    return returnable;
}