#pragma once

#include <SFML/Graphics.hpp>
#include "Comps.hpp"

/*
add<RenderHitboxes>(testTur, {sf::Color::Cyan});    col or later txtr
add<TurretWeaponLogic>(testTur, {300});             range

WeaponArsenal weaponArs;                            weapon stats
weaponArs.weapons.push_back(Weapon{});              
weaponArs.weapons[0].bulletLifetime = 5;
weaponArs.weapons[0].bulletRadius = 10;
weaponArs.weapons[0].bulletSpeed = 200;
weaponArs.weapons[0].bulletsShot = 1;
weaponArs.weapons[0].damage = 1;
weaponArs.weapons[0].dGroup = damageGroup::enemy;
weaponArs.weapons[0].fireRate = 1;
add<WeaponArsenal>(testTur, weaponArs);
*/


struct TurretStats
{
    sf::Color col; //replace with sprite when implemented
    int hp;
    int speed;
    float moveShootDelay;
    int friction;
    int radius;
    WeaponArsenal weapons; //THE RADIUS AND BULLET RADIUS GETS ADDED TO OFFSET.Y BY DEFAULT DURING CREATION (spawns in front of enemy)
    std::vector<int> ranges;
};

class TurretStatsManager
{
    TurretStatsManager() = delete;
    ~TurretStatsManager() = delete;
    private:
        

    public:

};