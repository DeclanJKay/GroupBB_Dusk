#include "Scenes.hpp"
#include "tile_level_loader/level_system.hpp"
#include "Comps.hpp"
#include <math.h>

using ls = LevelSystem;

void Scene::Update(const float& dt)
{
    _entMan.Update(dt);
}

void Scene::Draw(sf::RenderWindow& window)
{
    _entMan.Draw(window);
}

SafeHouse::SafeHouse(std::shared_ptr<Wallet> wallet, bool playerRestrict)
{
    player = _entMan.CreatePlayer();

    auto type = EnemyTypes::eBasic;
    //test enemy
    _entMan.CreateSHEnemy(&player, &type);

    //test shop
    auto shop = _entMan.CreateEntity();
    _entMan.add<Shop>(shop, Shop{});

    //player restriction
    RestrictPlayerEnt = _entMan.CreateEntity();
    _entMan.add<RestrictPlayerInput>(RestrictPlayerEnt, {playerRestrict}); 

    if (wallet == nullptr){return;}
    auto wlt = _entMan.CreateEntity();
    _entMan.add<WalletPtr>(wlt, {wallet});
}

void SafeHouse::Update(const float& dt, std::vector<EnemyTypes> toSpawn)
{
    Scene::Update(dt);
    for (int i = 0; i < toSpawn.size(); i++)
    {
        _entMan.CreateSHEnemy(&player, &toSpawn[i]);
    }
}

bool SafeHouse::NoEnemies()
{
    return _entMan.getAllEnt<EnemyType>().size() == 0;
}

bool SafeHouse::SetRestrictPlayer(bool b)
{
    if (!_entMan.Exists(RestrictPlayerEnt)) {return false;}
    _entMan.get<RestrictPlayerInput>(RestrictPlayerEnt)->restrict = b;
    return true;
}


//TOWER DEFENCE
TowerDefence::TowerDefence(std::shared_ptr<Wallet> wallet, bool playerRestrict)
{
    ls::set_color(ls::EMPTY, sf::Color(10, 10, 30));
    ls::set_color(ls::WALL, sf::Color(60, 60, 80));
    ls::set_color(ls::WAYPOINT, sf::Color(120, 120, 120));
    ls::set_color(ls::START, sf::Color(120, 120, 120));
    ls::set_color(ls::END, sf::Color(255, 80, 80));

    //sort path
    auto sorted = SortPath(ls::load_level("res/levels/td_1.txt", 50));
    sorted[0].x -= 50; 

    auto spawner = _entMan.CreateEntity();
    WaveSpawner spawnDef;
    spawnDef.canStart = true;
    spawnDef.iniPointBudget = 50;
    spawnDef.lvlIndex = -1; //lvl index gets increased after player starts wave, so set to -1 to begin at 0
    spawnDef.maxLvl = 5;
    spawnDef.path = sorted;
    spawnDef.pointIncrease = 20;
    spawnDef.spawnInterval = 1;
    spawnDef.waveIndex = 0; //im not really sure what this is even for
    _entMan.add<WaveSpawner>(spawner, spawnDef);

    auto turHandle = _entMan.CreateEntity();
    _entMan.add<TurretHandler>(turHandle, {});

    //player restriction
    RestrictPlayerEnt = _entMan.CreateEntity();
    _entMan.add<RestrictPlayerInput>(RestrictPlayerEnt, {playerRestrict}); 

    auto testText = _entMan.CreateEntity();
    sf::Text txt;
    txt.setString("Sigma testing");
    txt.setFont(*FileMgr::GetFont("res/fonts/ARIAL.TTF"));
    _entMan.add<Text>(testText, {5, txt});

    auto testBox = _entMan.CreateEntity();
    sf::RectangleShape rect;
    rect.setSize({100,200});
    _entMan.add<RectShape>(testBox, {4, rect});

    //create entity with reference to main wallet
    if (wallet == nullptr){return;}
    auto wlt = _entMan.CreateEntity();
    _entMan.add<WalletPtr>(wlt, {wallet});
}

std::vector<sf::Vector2f> TowerDefence:: SortPath(std::vector<sf::Vector2f> path)
{
    std::vector<sf::Vector2f> sorted;
    sorted.push_back(ls::get_start_position() + sf::Vector2f(25,25));
    int ogSize = path.size();
    for (int i = 1; i < ogSize; i++)
    {
        sf::Vector2f prevTile;
        prevTile = sorted.back();

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

void TowerDefence::Update(const float& dt, bool allEnemiesDead)
{
    Scene::Update(dt);
    auto pathEnts = _entMan.getAllEnt<TDPathMove>();
    for (auto ent : pathEnts)
    {
        if (_entMan.get<TDPathMove>(ent)->reachedEnd)
        {
            toTransfer.push_back(_entMan.get<EnemyType>(ent)->type);
        }
    }

    //enable spawner when all enemies are dead
    if (!allEnemiesDead) {return;}
    if (_entMan.getAllEnt<TDPathMove>().size() > 0) {return;}
    auto spawners = _entMan.getAllEnt<WaveSpawner>();
    for (auto spawner : spawners)
    {
        _entMan.get<WaveSpawner>(spawner)->canStart = true;
    }
}

bool TowerDefence::SetRestrictPlayer(bool b)
{
    if (!_entMan.Exists(RestrictPlayerEnt)) {return false;}
    _entMan.get<RestrictPlayerInput>(RestrictPlayerEnt)->restrict = b;
    return true;
}

std::vector<EnemyTypes> TowerDefence::GetTransfers()
{
    auto returnable = toTransfer;
    toTransfer.clear();
    return returnable;
}