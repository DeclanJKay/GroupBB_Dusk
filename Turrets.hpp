#pragma once

#include <SFML/Graphics.hpp>
#include "Comps.hpp"
#include "GenericHelpers.hpp"
#include <string>

struct TurretStats
{
    sf::Color col;
    WeaponArsenal weapons; //turrets only expect 1 weapon
    int range;
};

class TurretStatsManager
{
    TurretStatsManager() = delete;
    ~TurretStatsManager() = delete;
    private:
        static const std::unordered_map<Turrets, int> TurToCost;

    public:
        static std::map<int, std::vector<Turrets>> CostToTurrets();
        static TurretStats GetStats(Turrets turret);
        static std::string GetTurretName(Turrets turret);
        static int GetCost(Turrets turret);
};