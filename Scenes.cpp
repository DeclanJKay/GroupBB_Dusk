#include "Scenes.hpp"
#include "tile_level_loader/level_system.hpp"
#include "Comps.hpp"
#include <math.h>
#include "Prefabs.hpp"

using ls = LevelSystem;

void Scene::Update(const float& dt)
{
    _entMan.Update(dt);
}

void Scene::Draw(sf::RenderWindow& window)
{
    _entMan.Draw(window);
}

SafeHouse::SafeHouse()
{
    player = Prefabs::CreatePlayer(&_entMan);

    //test enemy
    auto enemy = _entMan.CreateEntity();
    _entMan.add<RenderHitboxes>(enemy, RenderHitboxes{sf::Color::White});
    _entMan.add<Position>(enemy, Position{sf::Vector2f(300, 100)});
    _entMan.add<Velocity>(enemy, Velocity{sf::Vector2f(0,0)});
    _entMan.add<Friction>(enemy, Friction{20});
    _entMan.add<CircleCollider>(enemy, CircleCollider{30});
    _entMan.add<Health>(enemy, {10, damageGroup::enemy});
    _entMan.add<EnemySafeMove>(enemy, EnemySafeMove{player, true, 50, {100, 400}});
    _entMan.add<EnemyShootingLogic>(enemy, EnemyShootingLogic{0.5f, player});
}

void SafeHouse::Update(const float& dt, std::vector<Entity> toSpawn)
{
    Scene::Update(dt);
    for (int i = 0; i < toSpawn.size(); i++)
    {
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

        auto enemy = _entMan.CreateEntity();
        _entMan.add<RenderHitboxes>(enemy, RenderHitboxes{sf::Color::White});
        _entMan.add<Position>(enemy, Position{sf::Vector2f(300, 100)});
        _entMan.add<Velocity>(enemy, Velocity{sf::Vector2f(0,0)});
        _entMan.add<Friction>(enemy, Friction{20});
        _entMan.add<CircleCollider>(enemy, CircleCollider{30});
        _entMan.add<Health>(enemy, {10, damageGroup::enemy});
        _entMan.add<EnemySafeMove>(enemy, EnemySafeMove{player, true, 50, {100, 400}});
        playerArsenal.weapons[0].dGroup = damageGroup::friendly;
        playerArsenal.weapons[1].dGroup = damageGroup::friendly;
        _entMan.add<WeaponArsenal>(enemy, playerArsenal);
        _entMan.add<EnemyShootingLogic>(enemy, EnemyShootingLogic{0.5f, player});
    }
}

//TOWER DEFENCE
TowerDefence::TowerDefence()
{
    ls::set_color(ls::EMPTY, sf::Color(10, 10, 30));
    ls::set_color(ls::WALL, sf::Color(60, 60, 80));
    ls::set_color(ls::WAYPOINT, sf::Color(120, 120, 120));
    ls::set_color(ls::START, sf::Color(120, 120, 120));
    ls::set_color(ls::END, sf::Color(255, 80, 80));
    ls::get_height();
    ls::get_width();

    //sort path
    auto sorted = SortPath(ls::load_level("res/levels/td_1.txt", 50));

    auto enemy = _entMan.CreateEntity();
    _entMan.add<Position>(enemy, {sorted[0]});
    _entMan.add<Health>(enemy, {5, damageGroup::enemy});
    _entMan.add<TDPathMove>(enemy, {false, 300, 1, sorted});
    _entMan.add<CircleCollider>(enemy, {30});
    _entMan.add<RenderHitboxes>(enemy, {sf::Color::White});
}

std::vector<sf::Vector2f> TowerDefence:: SortPath(std::vector<sf::Vector2f> path)
{
    std::vector<sf::Vector2f> sorted;
    int ogSize = path.size();
    for (int i = 0; i < ogSize; i++)
    {
        sf::Vector2f prevTile;
        if (i == 0) {prevTile = ls::get_start_position() + sf::Vector2f(25,25); }
        else { prevTile = sorted.back(); }

        int toRemove = -1;
        for (int x = 0; x < path.size(); x++)
        {
            auto dist = (path[x]+sf::Vector2f(25,25) - prevTile);
            dist.x = fabs(dist.x);
            dist.y = fabs(dist.y);

            if (dist.x + dist.y < 51) //means its adjecent
            {
                sorted.push_back(path[x] + sf::Vector2f(25,25));
                toRemove = x;
                break;
            }
        }
        path.erase(path.begin() + toRemove);
    }
    return sorted;
}

void TowerDefence::Update(const float& dt)
{
    Scene::Update(dt);
    auto pathEnts = _entMan.getAllEnt<TDPathMove>();
    for (auto ent : pathEnts)
    {
        if (_entMan.get<TDPathMove>(ent)->reachedEnd)
        {
            toTransfer.push_back(ent);
        }
    }
}

std::vector<Entity> TowerDefence::GetTransfers()
{
    auto returnable = toTransfer;
    toTransfer.clear();
    return returnable;
}