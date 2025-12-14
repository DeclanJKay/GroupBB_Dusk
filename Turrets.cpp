#include "Turrets.hpp"

const std::unordered_map<Turrets, int> TurretStatsManager::TurToCost = 
{
    {Turrets::tBasic, 1}, //again just random values
    {Turrets::tRapid, 4},
    {Turrets::tShotgun, 2},
    {Turrets::tSniper, 4}
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
        case Turrets::tBasic:               //random stats for basic, all copied over with the col changed
            stats.col = sf::Color::Cyan;
            stats.range = 300;
            stats.weapons.weapons.push_back({});
            stats.weapons.weapons[0].bulletLifetime = 5;
            stats.weapons.weapons[0].bulletRadius = 10;
            stats.weapons.weapons[0].bulletSpeed = 200;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].damage = 1;
            stats.weapons.weapons[0].dGroup = damageGroup::enemy;
            stats.weapons.weapons[0].fireRate = 1;
            break;

        case Turrets::tRapid:
            stats.col = sf::Color::Magenta;
            stats.range = 300;
            stats.weapons.weapons.push_back({});
            stats.weapons.weapons[0].bulletLifetime = 5;
            stats.weapons.weapons[0].bulletRadius = 10;
            stats.weapons.weapons[0].bulletSpeed = 200;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].damage = 1;
            stats.weapons.weapons[0].dGroup = damageGroup::enemy;
            stats.weapons.weapons[0].fireRate = 1;
            break;

        case Turrets::tShotgun:
            stats.col = sf::Color::Yellow;
            stats.range = 300;
            stats.weapons.weapons.push_back({});
            stats.weapons.weapons[0].bulletLifetime = 5;
            stats.weapons.weapons[0].bulletRadius = 10;
            stats.weapons.weapons[0].bulletSpeed = 200;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].damage = 1;
            stats.weapons.weapons[0].dGroup = damageGroup::enemy;
            stats.weapons.weapons[0].fireRate = 1;
            break;

        case Turrets::tSniper:
            stats.col = sf::Color::Red;
            stats.range = 300;
            stats.weapons.weapons.push_back({});
            stats.weapons.weapons[0].bulletLifetime = 5;
            stats.weapons.weapons[0].bulletRadius = 10;
            stats.weapons.weapons[0].bulletSpeed = 200;
            stats.weapons.weapons[0].bulletsShot = 1;
            stats.weapons.weapons[0].damage = 1;
            stats.weapons.weapons[0].dGroup = damageGroup::enemy;
            stats.weapons.weapons[0].fireRate = 1;
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

        case tRapid:
            return "Rapid";

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