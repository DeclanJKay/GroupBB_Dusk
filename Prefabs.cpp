#include "Prefabs.hpp"
#include "Comps.hpp"

void Prefabs::CreateSHEnemy(EntityManager* entMan, Entity* player)
{
    auto enemy = entMan->CreateEntity();
    entMan->add<RenderHitboxes>(enemy, RenderHitboxes{sf::Color::White});
    entMan->add<Position>(enemy, Position{sf::Vector2f(300, 100)});
    entMan->add<Velocity>(enemy, Velocity{sf::Vector2f(0,0)});
    entMan->add<Friction>(enemy, Friction{20});
    entMan->add<CircleCollider>(enemy, CircleCollider{30});
    entMan->add<Health>(enemy, {10, 10, damageGroup::enemy});
    entMan->add<EnemySafeMove>(enemy, EnemySafeMove{*player, 100, {100}});
    entMan->add<EnemyShootingLogic>(enemy, EnemyShootingLogic{0.5f, *player});

    WeaponArsenal enemyArs;
    enemyArs.selected = 0;
    enemyArs.weapons.push_back(Weapon{});
    enemyArs.weapons[0].bulletRadius = 10;
    enemyArs.weapons[0].bulletSpeed = 200;
    enemyArs.weapons[0].bulletsShot = 5;
    enemyArs.weapons[0].speedVariation = 100;
    enemyArs.weapons[0].bulletLifetime = 5;
    enemyArs.weapons[0].bulletSpread = 45;
    enemyArs.weapons[0].damage = 1;
    enemyArs.weapons[0].dGroup = damageGroup::friendly;
    enemyArs.weapons[0].fireRate = 2;
    enemyArs.weapons[0].pierce = 0;

    entMan->add<WeaponArsenal>(enemy, enemyArs);
}

void Prefabs::CreateTDEnemy(EntityManager* entMan, std::vector<sf::Vector2f> sorted)
{
    auto enemy = entMan->CreateEntity();
    entMan->add<Position>(enemy, {sorted[0]});
    entMan->add<Health>(enemy, {5, damageGroup::enemy});
    entMan->add<TDPathMove>(enemy, {false, 300, 1, sorted});
    entMan->add<CircleCollider>(enemy, {30});
    entMan->add<RenderHitboxes>(enemy, {sf::Color::White}); 
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
    entMan->add<Health>(player, {3, 3, friendly});
    entMan->add<CircleCollider>(player, CircleCollider{30});

    WeaponArsenal playerArs;

    playerArs.weapons.push_back(Weapon{});
    playerArs.weapons[0].bulletRadius = 10;
    playerArs.weapons[0].bulletSpeed = 200;
    playerArs.weapons[0].bulletsShot = 5;
    playerArs.weapons[0].speedVariation = 100;
    playerArs.weapons[0].bulletLifetime = 5;
    playerArs.weapons[0].bulletSpread = 45;
    playerArs.weapons[0].damage = 1;
    playerArs.weapons[0].dGroup = damageGroup::enemy;
    playerArs.weapons[0].fireRate = 2;
    playerArs.weapons[0].pierce = 0;
    entMan->add<WeaponArsenal>(player, playerArs);
    entMan->add<PlayerWeaponLogic>(player,{4});

    return player;
}