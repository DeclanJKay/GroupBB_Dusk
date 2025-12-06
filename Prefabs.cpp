#include "Prefabs.hpp"
#include "Comps.hpp"

void Prefabs::CreateSHEnemy(EntityManager* entman)
{

}

void Prefabs::CreateTDEnemy(EntityManager* entman)
{

}

Entity Prefabs::CreatePlayer(EntityManager* entMan)
{
//add other components to the player
    auto player = entMan->CreateEntity();
    entMan->add<RenderHitboxes>(player, RenderHitboxes{sf::Color::White});
    entMan->add<PlayerMovement>(player, PlayerMovement{100});
    entMan->add<Position>(player, Position{sf::Vector2f(300,300)});
    entMan->add<Velocity>(player, Velocity{sf::Vector2f(0,0)});
    entMan->add<Friction>(player, Friction{20});
    entMan->add<Health>(player, {3, friendly});
    entMan->add<CircleCollider>(player, CircleCollider{30});

    WeaponArsenal playerArsenal;

    playerArsenal.weapons.push_back(Weapon{});
    playerArsenal.weapons[0].bulletRadius = 10;
    playerArsenal.weapons[0].bulletSpeed = 200;
    playerArsenal.weapons[0].bulletsShot = 5;
    playerArsenal.weapons[0].speedVariation = 100;
    playerArsenal.weapons[0].bulletLifetime = 5;
    playerArsenal.weapons[0].bulletSpread = 45;
    playerArsenal.weapons[0].damage = 1;
    playerArsenal.weapons[0].dGroup = damageGroup::enemy;
    playerArsenal.weapons[0].fireRate = 2;
    playerArsenal.weapons[0].pierce = 0;

    playerArsenal.weapons.push_back(Weapon{});
    playerArsenal.weapons[1].bulletRadius = 20;
    playerArsenal.weapons[1].bulletSpeed = 5;
    playerArsenal.weapons[1].bulletsShot = 1;
    playerArsenal.weapons[1].bulletLifetime = 1;
    playerArsenal.weapons[1].damage = 1;
    playerArsenal.weapons[1].dGroup = damageGroup::enemy;
    playerArsenal.weapons[1].fireRate = 10;
    playerArsenal.weapons[1].pierce = 0;

    entMan->add<WeaponArsenal>(player, playerArsenal);
    entMan->add<PlayerWeaponLogic>(player,{});

    return player;
}