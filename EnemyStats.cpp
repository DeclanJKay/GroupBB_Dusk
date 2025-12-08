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
            stats.speed = 60.f;
            stats.radius = 15.f;
            stats.col = sf::Color::Red;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;

            stats.ranges.push_back(120);

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 1; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 100;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].bulletLifetime = 2;
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

            stats.ranges.push_back(30);

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 1; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 0;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].bulletLifetime = 0.5f;
            stats.weapons.weapons[0].damage = 1;
            stats.weapons.weapons[0].bulletRadius = 30;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
        break;

        case EnemyTypes::Tank:
            stats.hp = 6;
            stats.speed = 40.f;
            stats.radius = 18.f;
            stats.col = sf::Color(150, 0, 200); // purple
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;

            stats.ranges.push_back(150);

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 0.8f; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 70;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].bulletLifetime = 3;
            stats.weapons.weapons[0].damage = 2;
            stats.weapons.weapons[0].bulletRadius = 20;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            break;

            // ----------------- LEVEL 2 UNLOCKS -----------------
        case EnemyTypes::shortRanged:
            stats.hp = 4;
            stats.speed = 70.f;
            stats.radius = 14.f;
            stats.col = sf::Color::Black;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;

            stats.ranges.push_back(120);

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 0.5f; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 100;
            stats.weapons.weapons[0].bulletsShot = 3;
            stats.weapons.weapons[0].bulletLifetime = 3;
            stats.weapons.weapons[0].bulletSpread = 30;
            stats.weapons.weapons[0].damage = 1;
            stats.weapons.weapons[0].bulletRadius = 20;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            break;

        case EnemyTypes::Exploder:
            stats.hp = 2;
            stats.speed = 80.f;
            stats.radius = 14.f;
            stats.col = sf::Color::Blue;
            //stats.explodes = true;
            stats.friction = 20;
            stats.moveShootDelay = 3;

            stats.ranges.push_back(50);

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 0.5f; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 0;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].bulletLifetime = 0.5f;
            stats.weapons.weapons[0].damage = 3;
            stats.weapons.weapons[0].bulletRadius = 80;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            break;

            // ----------------- LEVEL 3 UNLOCKS -----------------
        case EnemyTypes::Medium:
            stats.hp = 4;
            stats.speed = 75.f;
            stats.radius = 14.f;
            stats.col = sf::Color::Green;
            stats.friction = 20;
            stats.moveShootDelay = 0.8f;

            stats.ranges.push_back(30);

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 1; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 30;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].bulletLifetime = 0.5f;
            stats.weapons.weapons[0].damage = 2;
            stats.weapons.weapons[0].bulletRadius = 40;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            break;

        case EnemyTypes::RangedMelee:
            stats.hp = 5;
            stats.speed = 70.f;
            stats.radius = 15.f;
            stats.col = sf::Color::Magenta;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;

            stats.ranges = {150, 30};

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 2; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 100;
            stats.weapons.weapons[0].bulletSpread = 45;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].bulletLifetime = 3;
            stats.weapons.weapons[0].damage = 1;
            stats.weapons.weapons[0].bulletRadius = 10;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;

            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[1].fireRate = 1; //bullets per second
            stats.weapons.weapons[1].bulletSpeed = 30;
            stats.weapons.weapons[1].bulletsShot = 1;
            stats.weapons.weapons[1].bulletLifetime = 0.5f;
            stats.weapons.weapons[1].damage = 2;
            stats.weapons.weapons[1].bulletRadius = 30;
            stats.weapons.weapons[1].dGroup = damageGroup::friendly;
            break;

        case EnemyTypes::FastExploder:
            stats.hp = 2;
            stats.speed = 120.f;
            stats.radius = 13.f;
            stats.col = sf::Color::Cyan;
            //stats.explodes = true;
            stats.friction = 20;
            stats.moveShootDelay = 3;

            stats.ranges = {50};

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 1; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 0;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].bulletLifetime = 0.5f;
            stats.weapons.weapons[0].damage = 3;
            stats.weapons.weapons[0].bulletRadius = 90;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            break;

            // ----------------- LEVEL 4 UNLOCKS -----------------
        case EnemyTypes::LongRange:
            stats.hp = 3;
            stats.speed = 65.f;
            stats.radius = 13.f;
            stats.col = sf::Color(100, 200, 255);
            //stats.damage = 2;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;

            stats.ranges = {220};

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 1.5f; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 150;
            stats.weapons.weapons[0].bulletSpread = 10;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].bulletLifetime = 3;
            stats.weapons.weapons[0].damage = 2;
            stats.weapons.weapons[0].bulletRadius = 10;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            break;

        case EnemyTypes::HeavyTank:
            stats.hp = 10;
            stats.speed = 35.f;
            stats.radius = 20.f;
            stats.col = sf::Color(80, 80, 80);
            stats.friction = 20;
            stats.moveShootDelay = 1;

            stats.ranges = {60};

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 2; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 50;
            stats.weapons.weapons[0].bulletSpread = 45;
            stats.weapons.weapons[0].bulletsShot = 5;
            stats.weapons.weapons[0].speedVariation = 30;
            stats.weapons.weapons[0].bulletLifetime = 1.5f;
            stats.weapons.weapons[0].damage = 3;
            stats.weapons.weapons[0].bulletRadius = 20;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
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
    for (int i = 0; i <= std::min(levelInd, (int)enemiesPerLevel->size()-1); i++)
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

std::vector<int> EnemyStatsManager::GetSortedKeys(std::map<int, std::vector<EnemyTypes>>* costMap)
{
    std::vector<int> IndexToKey;
    for (auto pair : *costMap)
    {
        IndexToKey.push_back(pair.first);
    }
    return IndexToKey;
}
