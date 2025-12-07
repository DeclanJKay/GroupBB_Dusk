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
    player = _entMan.CreatePlayer();

    //test enemy
    _entMan.CreateSHEnemy(&player);
}

void SafeHouse::Update(const float& dt, std::vector<Entity> toSpawn)
{
    Scene::Update(dt);
    for (int i = 0; i < toSpawn.size(); i++)
    {
        _entMan.CreateSHEnemy(&player);
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

    //sort path
    auto sorted = SortPath(ls::load_level("res/levels/td_1.txt", 50));

    //test enemy
    _entMan.CreateTDEnemy(sorted);

    auto spawner = _entMan.CreateEntity();
    _entMan.add<WaveSpawner>(spawner, WaveSpawner{0,0, 50, 1, 1, sorted});
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