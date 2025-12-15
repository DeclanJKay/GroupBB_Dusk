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
    {EnemyTypes::eExploder, 3},
    {EnemyTypes::eMedium, 3},
    {EnemyTypes::eRangedMelee, 4},
    {EnemyTypes::eFastExploder, 4},
    {EnemyTypes::eLongRange, 4},
    {EnemyTypes::eHeavyTank, 5}
};

const std::vector<EnemyTypes> EnemyStatsManager::enemiesPerLevel[]
{
    //level 1
    {
        EnemyTypes::eBasic,
        EnemyTypes::eFast,
        EnemyTypes::eTank
    },

    //level 2
    {
        EnemyTypes::eShortRanged, 
        EnemyTypes::eExploder
    },

    //level 3
    {
        EnemyTypes::eMedium,
        EnemyTypes::eRangedMelee, 
        EnemyTypes::eFastExploder
    },

    //level 4
    {
        EnemyTypes::eLongRange,
        EnemyTypes::eHeavyTank
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
            stats.hp = 3;
            stats.speed = 60.f;
            stats.radius = 15.f;
            stats.txtr = FileMgr::GetTxtr("res/img/tempBody.png");
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

        case EnemyTypes::eFast:
            stats.hp = 2;
            stats.speed = 110.f;
            stats.radius = 12.f;
            stats.txtr = FileMgr::GetTxtr("res/img/tempBody.png");
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

        case EnemyTypes::eTank:
            stats.hp = 6;
            stats.speed = 40.f;
            stats.radius = 18.f;
            stats.txtr = FileMgr::GetTxtr("res/img/tempBody.png");
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
        case EnemyTypes::eShortRanged:
            stats.hp = 4;
            stats.speed = 70.f;
            stats.radius = 14.f;
            stats.txtr = FileMgr::GetTxtr("res/img/tempBody.png");
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

        case EnemyTypes::eExploder:
            stats.hp = 2;
            stats.speed = 80.f;
            stats.radius = 14.f;
            stats.txtr = FileMgr::GetTxtr("res/img/tempBody.png");
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
        case EnemyTypes::eMedium:
            stats.hp = 4;
            stats.speed = 75.f;
            stats.radius = 14.f;
            stats.txtr = FileMgr::GetTxtr("res/img/tempBody.png");
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

        case EnemyTypes::eRangedMelee:
            stats.hp = 5;
            stats.speed = 70.f;
            stats.radius = 15.f;
            stats.txtr = FileMgr::GetTxtr("res/img/tempBody.png");
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

        case EnemyTypes::eFastExploder:
            stats.hp = 2;
            stats.speed = 120.f;
            stats.radius = 13.f;
            stats.txtr = FileMgr::GetTxtr("res/img/tempBody.png");
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
        case EnemyTypes::eLongRange:
            stats.hp = 3;
            stats.speed = 65.f;
            stats.radius = 13.f;
            stats.txtr = FileMgr::GetTxtr("res/img/tempBody.png");
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

        case EnemyTypes::eHeavyTank:
            stats.hp = 10;
            stats.speed = 35.f;
            stats.radius = 20.f;
            stats.txtr = FileMgr::GetTxtr("res/img/tempBody.png");
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
        case EnemyTypes::eBoss1:
            stats.hp = 10;
            stats.speed = 80.f;
            stats.radius = 20.f;
            stats.txtr = FileMgr::GetTxtr("res/img/tempBody.png");
            stats.friction = 20;
            stats.moveShootDelay = 1;
            
            stats.shieldAmount = 20;

            //weapon
            stats.ranges = {200, 200};
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 5; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 100;
            stats.weapons.weapons[0].bulletSpread = 45;
            stats.weapons.weapons[0].bulletsShot = 5;
            stats.weapons.weapons[0].bulletLifetime = 5;
            stats.weapons.weapons[0].damage = 3;
            stats.weapons.weapons[0].bulletRadius = 20;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            stats.weapons.weapons[0].gunTxtr = FileMgr::GetTxtr("res/img/gun.png");

            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[1].fireRate = 1; //bullets per second
            stats.weapons.weapons[1].bulletSpeed = 100;
            stats.weapons.weapons[1].bulletSpread = 45;
            stats.weapons.weapons[1].bulletsShot = 5;
            stats.weapons.weapons[1].bulletLifetime = 5;
            stats.weapons.weapons[1].damage = 3;
            stats.weapons.weapons[1].bulletRadius = 20;
            stats.weapons.weapons[1].dGroup = damageGroup::friendly;
            stats.weapons.weapons[1].gunTxtr = FileMgr::GetTxtr("res/img/gun.png");
            break;

        case EnemyTypes::eBoss2:
            stats.hp = 10;
            stats.speed = 80.f;
            stats.radius = 20.f;
            stats.txtr = FileMgr::GetTxtr("res/img/tempBody.png");
            stats.friction = 20;
            stats.moveShootDelay = 1;
            
            stats.shieldAmount = 20;

            //weapon
            stats.ranges = {200, 200};
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 5; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 100;
            stats.weapons.weapons[0].bulletSpread = 45;
            stats.weapons.weapons[0].bulletsShot = 5;
            stats.weapons.weapons[0].bulletLifetime = 5;
            stats.weapons.weapons[0].damage = 3;
            stats.weapons.weapons[0].bulletRadius = 20;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            stats.weapons.weapons[0].gunTxtr = FileMgr::GetTxtr("res/img/gun.png");

            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[1].fireRate = 1; //bullets per second
            stats.weapons.weapons[1].bulletSpeed = 100;
            stats.weapons.weapons[1].bulletSpread = 45;
            stats.weapons.weapons[1].bulletsShot = 5;
            stats.weapons.weapons[1].bulletLifetime = 5;
            stats.weapons.weapons[1].damage = 3;
            stats.weapons.weapons[1].bulletRadius = 20;
            stats.weapons.weapons[1].dGroup = damageGroup::friendly;
            stats.weapons.weapons[1].gunTxtr = FileMgr::GetTxtr("res/img/gun.png");
            break;

        case EnemyTypes::eBoss3:
            stats.hp = 10;
            stats.speed = 80.f;
            stats.radius = 20.f;
            stats.txtr = FileMgr::GetTxtr("res/img/tempBody.png");
            stats.friction = 20;
            stats.moveShootDelay = 1;
            
            stats.shieldAmount = 20;

            //weapon
            stats.ranges = {200, 200};
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 5; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 100;
            stats.weapons.weapons[0].bulletSpread = 45;
            stats.weapons.weapons[0].bulletsShot = 5;
            stats.weapons.weapons[0].bulletLifetime = 5;
            stats.weapons.weapons[0].damage = 3;
            stats.weapons.weapons[0].bulletRadius = 20;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            stats.weapons.weapons[0].gunTxtr = FileMgr::GetTxtr("res/img/gun.png");

            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[1].fireRate = 1; //bullets per second
            stats.weapons.weapons[1].bulletSpeed = 100;
            stats.weapons.weapons[1].bulletSpread = 45;
            stats.weapons.weapons[1].bulletsShot = 5;
            stats.weapons.weapons[1].bulletLifetime = 5;
            stats.weapons.weapons[1].damage = 3;
            stats.weapons.weapons[1].bulletRadius = 20;
            stats.weapons.weapons[1].dGroup = damageGroup::friendly;
            stats.weapons.weapons[1].gunTxtr = FileMgr::GetTxtr("res/img/gun.png");
            break;

        case EnemyTypes::eBoss4:
            stats.hp = 10;
            stats.speed = 80.f;
            stats.radius = 20.f;
            stats.txtr = FileMgr::GetTxtr("res/img/tempBody.png");
            stats.friction = 20;
            stats.moveShootDelay = 1;
            
            stats.shieldAmount = 20;

            //weapon
            stats.ranges = {200, 200};
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 5; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 100;
            stats.weapons.weapons[0].bulletSpread = 45;
            stats.weapons.weapons[0].bulletsShot = 5;
            stats.weapons.weapons[0].bulletLifetime = 5;
            stats.weapons.weapons[0].damage = 3;
            stats.weapons.weapons[0].bulletRadius = 20;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            stats.weapons.weapons[0].gunTxtr = FileMgr::GetTxtr("res/img/gun.png");

            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[1].fireRate = 1; //bullets per second
            stats.weapons.weapons[1].bulletSpeed = 100;
            stats.weapons.weapons[1].bulletSpread = 45;
            stats.weapons.weapons[1].bulletsShot = 5;
            stats.weapons.weapons[1].bulletLifetime = 5;
            stats.weapons.weapons[1].damage = 3;
            stats.weapons.weapons[1].bulletRadius = 20;
            stats.weapons.weapons[1].dGroup = damageGroup::friendly;
            stats.weapons.weapons[1].gunTxtr = FileMgr::GetTxtr("res/img/gun.png");
            break;

        case EnemyTypes::eBoss5:
            stats.hp = 10;
            stats.speed = 80.f;
            stats.radius = 20.f;
            stats.txtr = FileMgr::GetTxtr("res/img/tempBody.png");
            stats.friction = 20;
            stats.moveShootDelay = 1;
            
            stats.shieldAmount = 20;

            //weapon
            stats.ranges = {200, 200};
            stats.weapons = WeaponArsenal{};
            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[0].fireRate = 5; //bullets per second
            stats.weapons.weapons[0].bulletSpeed = 100;
            stats.weapons.weapons[0].bulletSpread = 45;
            stats.weapons.weapons[0].bulletsShot = 5;
            stats.weapons.weapons[0].bulletLifetime = 5;
            stats.weapons.weapons[0].damage = 3;
            stats.weapons.weapons[0].bulletRadius = 20;
            stats.weapons.weapons[0].dGroup = damageGroup::friendly;
            stats.weapons.weapons[0].gunTxtr = FileMgr::GetTxtr("res/img/gun.png");

            stats.weapons.weapons.push_back(Weapon{});
            stats.weapons.weapons[1].fireRate = 1; //bullets per second
            stats.weapons.weapons[1].bulletSpeed = 100;
            stats.weapons.weapons[1].bulletSpread = 45;
            stats.weapons.weapons[1].bulletsShot = 5;
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
        EnemyTypes::eBoss4,
        EnemyTypes::eBoss5,
    };

    return bosses[rand()%(sizeof(bosses)/sizeof(EnemyTypes))];
}