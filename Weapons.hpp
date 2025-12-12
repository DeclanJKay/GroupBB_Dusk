#pragma once

#include "Comps.hpp"
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "TextureManager.hpp"

class WeaponStatsMgr
{
    WeaponStatsMgr() = delete;
    ~WeaponStatsMgr() = delete;

    private:
        static const std::unordered_map<Weapons, int> WeaponCosts; //for the shop

    public:
        static Weapon GetStats(Weapons weapon);
        static int GetCost(Weapons weapon);
};