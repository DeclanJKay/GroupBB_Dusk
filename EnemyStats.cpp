#include "EnemyStats.hpp"
#include "Comps.hpp"
#include <iterator>
#include "FileMgr.hpp"

const std::unordered_map<EnemyTypes, int> EnemyStatsManager::EnemyToCost
{
    {EnemyTypes::eBasic, 1},
    {EnemyTypes::eFast, 2},
    {EnemyTypes::eTank, 3},
    {EnemyTypes::eShortRanged, 3},
    {EnemyTypes::eMedium, 3},
    {EnemyTypes::eRangedMelee, 4},
    {EnemyTypes::eLongRange, 4},
};

const std::vector<EnemyTypes> EnemyStatsManager::enemiesPerLevel[]
{
    //level 1
    {
        EnemyTypes::eBasic,
        EnemyTypes::eTank
    },

    //level 2
    {
        EnemyTypes::eShortRanged, 
        EnemyTypes::eFast,
    },

    //level 3
    {
        EnemyTypes::eMedium,
        EnemyTypes::eRangedMelee, 
    },

    //level 4
    {
        EnemyTypes::eLongRange,
    }
};

//assign stats based on enemy type given, and returns the struct
EnemyStats EnemyStatsManager::GetStats(EnemyTypes type)
{
    //to add weapons, range 
    EnemyStats stats;
    switch (type)
    {
        case EnemyTypes::eBasic:
            //basic stats
            stats.hp = 5;
            stats.speed = 60.f;
            stats.radius = 15.f;
            stats.txtr = FileMgr::GetTxtr("res/img/Enemy1.png");
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;

            stats.ranges.push_back(120);

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 1; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 300;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].bulletLifetime = 2;
            stats.weapons.weapons[0].damage = 1;
            stats.weapons.weapons[0].bulletRadius = 10;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
        break;

        case EnemyTypes::eFast:
            stats.hp = 3;
            stats.speed = 110.f;
            stats.radius = 12.f;
            stats.txtr = FileMgr::GetTxtr("res/img/Enemy2.png");
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;

            stats.ranges.push_back(40);

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 1; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 0;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].bulletLifetime = 0.5f;
            stats.weapons.weapons[0].damage = 2;
            stats.weapons.weapons[0].bulletRadius = 30;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
        break;

        case EnemyTypes::eTank:
            stats.hp = 8;
            stats.speed = 50.f;
            stats.radius = 20.f;
            stats.txtr = FileMgr::GetTxtr("res/img/Enemy3.png");
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;

            stats.ranges.push_back(300);

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 0.8f; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 300;
            stats.weapons.weapons[0].bulletsShot = 2;
            stats.weapons.weapons[0].bulletSpread = 30;
            stats.weapons.weapons[0].bulletLifetime = 3;
            stats.weapons.weapons[0].damage = 2;
            stats.weapons.weapons[0].bulletRadius = 20;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            break;

            // ----------------- LEVEL 2 UNLOCKS -----------------
        case EnemyTypes::eShortRanged:
            stats.hp = 6;
            stats.speed = 70.f;
            stats.radius = 14.f;
            stats.txtr = FileMgr::GetTxtr("res/img/Enemy4.png");
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;

            stats.ranges.push_back(300);

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 2; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 400;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].bulletLifetime = 3;
            stats.weapons.weapons[0].bulletSpread = 20;
            stats.weapons.weapons[0].damage = 1;
            stats.weapons.weapons[0].bulletRadius = 20;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            break;

            // ----------------- LEVEL 3 UNLOCKS -----------------
        case EnemyTypes::eMedium:
            stats.hp = 7;
            stats.speed = 75.f;
            stats.radius = 14.f;
            stats.txtr = FileMgr::GetTxtr("res/img/Enemy5.png");
            stats.friction = 20;
            stats.moveShootDelay = 0.8f;

            stats.ranges.push_back(500);

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 1.5f; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 450;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].bulletLifetime = 2;
            stats.weapons.weapons[0].damage = 2;
            stats.weapons.weapons[0].bulletRadius = 40;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            break;

        case EnemyTypes::eRangedMelee:
            stats.hp = 9;
            stats.speed = 70.f;
            stats.radius = 15.f;
            stats.txtr = FileMgr::GetTxtr("res/img/Enemy6.png");
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;

            stats.ranges = {300, 40};

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 1; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 500;
            stats.weapons.weapons[0].bulletSpread = 5;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].bulletLifetime = 0.5f;
            stats.weapons.weapons[0].damage = 2;
            stats.weapons.weapons[0].bulletRadius = 10;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;

            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[1].fireRate = 1; //bullets per second
            stats.weapons.weapons[1].bulletSpeed = 30;
            stats.weapons.weapons[1].bulletsShot = 1;
            stats.weapons.weapons[1].bulletLifetime = 0.5f;
            stats.weapons.weapons[1].damage = 3;
            stats.weapons.weapons[1].bulletRadius = 30;
            stats.weapons.weapons[1].dGroup = damageGroup::friendly;
            break;

            // ----------------- LEVEL 4 UNLOCKS -----------------
        case EnemyTypes::eLongRange:
            stats.hp = 6;
            stats.speed = 65.f;
            stats.radius = 13.f;
            stats.txtr = FileMgr::GetTxtr("res/img/Enemy7.png");
            //stats.damage = 2;
            stats.friction = 20;
            stats.moveShootDelay = 0.5f;

            stats.ranges = {700};

            //weapon
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 0.8f; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 700;
            stats.weapons.weapons[0].bulletSpread = 5;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].bulletLifetime = 3;
            stats.weapons.weapons[0].damage = 4;
            stats.weapons.weapons[0].bulletRadius = 10;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            break;

            // ----------------- BOSSES (ONE PER LEVEL) -----------------
        case EnemyTypes::eBoss1:
            stats.hp = 10;
            stats.speed = 60.f;
            stats.radius = 20.f;
            stats.txtr = FileMgr::GetTxtr("res/img/Enemy8.png");
            stats.friction = 20;
            stats.moveShootDelay = 1;
            
            stats.shieldAmount = 30;

            //weapon
            stats.ranges = {400, 400};
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 3; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 400;
            stats.weapons.weapons[0].bulletSpread = 30;
            stats.weapons.weapons[0].bulletsShot = 3;
            stats.weapons.weapons[0].bulletLifetime = 5;
            stats.weapons.weapons[0].damage = 1;
            stats.weapons.weapons[0].bulletRadius = 20;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            stats.weapons.weapons[0].gunTxtr = FileMgr::GetTxtr("res/img/gun.png");

            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[1].fireRate = 1; //bullets per second
            stats.weapons.weapons[1].bulletSpeed = 400;
            stats.weapons.weapons[1].bulletSpread = 30;
            stats.weapons.weapons[1].bulletsShot = 3;
            stats.weapons.weapons[1].bulletLifetime = 5;
            stats.weapons.weapons[1].damage = 1;
            stats.weapons.weapons[1].bulletRadius = 20;
            stats.weapons.weapons[1].dGroup = damageGroup::friendly;
            stats.weapons.weapons[1].gunTxtr = FileMgr::GetTxtr("res/img/gun.png");
            break;

        case EnemyTypes::eBoss2:
            stats.hp = 5;
            stats.speed = 90.f;
            stats.radius = 20.f;
            stats.txtr = FileMgr::GetTxtr("res/img/Enemy9.png");
            stats.friction = 20;
            stats.moveShootDelay = 1;
            
            stats.shieldAmount = 20;

            //weapon
            stats.ranges = {300, 300};
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 5; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 500;
            stats.weapons.weapons[0].bulletSpread = 45;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].bulletLifetime = 5;
            stats.weapons.weapons[0].damage = 1;
            stats.weapons.weapons[0].bulletRadius = 20;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            stats.weapons.weapons[0].gunTxtr = FileMgr::GetTxtr("res/img/gun.png");

            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[1].fireRate = 1; //bullets per second
            stats.weapons.weapons[1].bulletSpeed = 400;
            stats.weapons.weapons[1].bulletSpread = 360;
            stats.weapons.weapons[1].bulletsShot = 10;
            stats.weapons.weapons[1].bulletLifetime = 3;
            stats.weapons.weapons[1].damage = 2;
            stats.weapons.weapons[1].bulletRadius = 20;
            stats.weapons.weapons[1].dGroup = damageGroup::friendly;
            stats.weapons.weapons[1].gunTxtr = FileMgr::GetTxtr("res/img/gun.png");
            break;

        case EnemyTypes::eBoss3:
            stats.hp = 15;
            stats.speed = 50.f;
            stats.radius = 20.f;
            stats.txtr = FileMgr::GetTxtr("res/img/Enemy10.png");
            stats.friction = 20;
            stats.moveShootDelay = 1;
            
            stats.shieldAmount = 45;

            //weapon
            stats.ranges = {700, 700};
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 0.6f; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 700;
            stats.weapons.weapons[0].bulletSpread = 20;
            stats.weapons.weapons[0].bulletsShot = 2;
            stats.weapons.weapons[0].bulletLifetime = 5;
            stats.weapons.weapons[0].damage = 2;
            stats.weapons.weapons[0].bulletRadius = 20;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            stats.weapons.weapons[0].gunTxtr = FileMgr::GetTxtr("res/img/gun.png");

            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[1].fireRate = 1; //bullets per second
            stats.weapons.weapons[1].bulletSpeed = 700;
            stats.weapons.weapons[1].bulletSpread = 0;
            stats.weapons.weapons[1].bulletsShot = 1;
            stats.weapons.weapons[1].bulletLifetime = 5;
            stats.weapons.weapons[1].damage = 3;
            stats.weapons.weapons[1].bulletRadius = 20;
            stats.weapons.weapons[1].dGroup = damageGroup::friendly;
            stats.weapons.weapons[1].gunTxtr = FileMgr::GetTxtr("res/img/gun.png");
            break;
    }
    return stats;
}

int EnemyStatsManager::GetCost(EnemyTypes type)
{
    if (!EnemyToCost.contains(type)) {return -1;} //returns -1 if invalid
    return EnemyToCost.at(type);
}

std::map<int, std::vector<EnemyTypes>> EnemyStatsManager::GetLevelCostMap(int levelInd)
{
    //returns a map linking cost to a list of enemy types
    //similar to FlipMap() in general helpers
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

EnemyTypes EnemyStatsManager::GetBoss()
{
    const EnemyTypes bosses[] = 
    {
        EnemyTypes::eBoss1,
        EnemyTypes::eBoss2,
        EnemyTypes::eBoss3,
    };

    return bosses[rand()%(sizeof(bosses)/sizeof(EnemyTypes))];
}