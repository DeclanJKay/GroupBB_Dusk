#include "Turrets.hpp"
#include "FileMgr.hpp"

const std::unordered_map<Turrets, int> TurretStatsManager::TurToCost = 
{
    {Turrets::tBasic, 4}, 
    {Turrets::tBigGun, 7},
    {Turrets::tShotgun, 8},
    {Turrets::tSniper, 10}
};

//todo: the readable storage version above isn't really needed past game runtime. figure out a way to make this
//      more efficient.
std::map<int, std::vector<Turrets>> TurretStatsManager::CostToTurrets()
{
    return FlipMap(TurToCost);
}

TurretStats TurretStatsManager::GetStats(Turrets turret)
{
    TurretStats stats;
    switch (turret)
    {
        case Turrets::tBasic:               //random stats for basic, all copied over with the.txtr = FileMgr::GetTxtr("res/img/tempShopEntry.png");
            stats.txtr = FileMgr::GetTxtr("res/img/BasicTurret.png");
            stats.range = 150;
            stats.weapons.weapons.push_back({});
            stats.weapons.weapons[0].bulletLifetime = 3;
            stats.weapons.weapons[0].bulletRadius = 10;
            stats.weapons.weapons[0].bulletSpeed = 200;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].damage = 2;
            stats.weapons.weapons[0].dGroup = damageGroup::enemy;
            stats.weapons.weapons[0].fireRate = 1;
            break;

        case Turrets::tBigGun:
            stats.txtr = FileMgr::GetTxtr("res/img/FastTurret.png");
            stats.range = 300;
            stats.weapons.weapons.push_back({});
            stats.weapons.weapons[0].bulletLifetime = 5;
            stats.weapons.weapons[0].bulletRadius = 50;
            stats.weapons.weapons[0].bulletSpeed = 100;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].damage = 3;
            stats.weapons.weapons[0].dGroup = damageGroup::enemy;
            stats.weapons.weapons[0].fireRate = 0.5f;
            break;

        case Turrets::tShotgun:
            stats.txtr = FileMgr::GetTxtr("res/img/ShotgunTurret.png");
            stats.range = 100;
            stats.weapons.weapons.push_back({});
            stats.weapons.weapons[0].bulletLifetime = 1;
            stats.weapons.weapons[0].bulletRadius = 10;
            stats.weapons.weapons[0].bulletSpeed = 300;
            stats.weapons.weapons[0].bulletsShot = 4;
            stats.weapons.weapons[0].bulletSpread = 45;
            stats.weapons.weapons[0].speedVariation = 30;
            stats.weapons.weapons[0].damage = 1;
            stats.weapons.weapons[0].dGroup = damageGroup::enemy;
            stats.weapons.weapons[0].fireRate = 0.8f;
            break;

        case Turrets::tSniper:
            stats.txtr = FileMgr::GetTxtr("res/img/SniperTurret.png");
            stats.range = 500;
            stats.weapons.weapons.push_back({});
            stats.weapons.weapons[0].bulletLifetime = 7;
            stats.weapons.weapons[0].bulletRadius = 10;
            stats.weapons.weapons[0].bulletSpeed = 700;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].damage = 3;
            stats.weapons.weapons[0].dGroup = damageGroup::enemy;
            stats.weapons.weapons[0].fireRate = 0.7f;
            break;
    }
    return stats;
}

std::string TurretStatsManager::GetTurretName(Turrets turret)
{
    switch (turret)
    {
        case tBasic:
            return "Basic";

        case tBigGun:
            return "BigGun";

        case tShotgun:
            return "Shotgun";

        case tSniper:
            return "Sniper";
    }
}

int TurretStatsManager::GetCost(Turrets turret)
{
    return TurToCost.at(turret);
}