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


//SHOP
ShopScene::ShopScene(std::shared_ptr<Wallet> wallet)
{
    if (wallet == nullptr){return;}

    //create instance with wallet ptr
    auto wall = _entMan.CreateEntity();
    _entMan.add<WalletPtr>(wall, {wallet});

    //create shop
    auto shop = _entMan.CreateEntity();
    _entMan.add<Shop>(shop, Shop{});

    _entMan.PopulateShops(3);

    //create money display
    totalMoney = _entMan.CreateEntity();
    sf::Text txt;
    txt.setFont(*FileMgr::GetFont("res/fonts/ARIAL.TTF"));
    txt.setString(std::to_string(wallet->money));
    _entMan.add<Text>(totalMoney, {1,txt});

    CreateShopEnts();
    //test button
    /*
    auto button = _entMan.CreateEntity();
    _entMan.add<Position>(button, {{600, 300}});
    sf::RectangleShape shape;
    shape.setSize({300,100});
    shape.setOrigin({150, 50});
    _entMan.add<RectShape>(button, {10,shape});
    _entMan.add<Button>(button, {{300,100},false,false});
    sf::Text txt2;
    txt2.setFont(*FileMgr::GetFont("res/fonts/ARIAL.TTF"));
    txt2.setString("Epic Button");
    txt2.setColor(sf::Color::Black);
    txt2.setOrigin(txt2.getGlobalBounds().getSize()/2.f);
    _entMan.add<Text>(button, {11, txt2});*/
}

void ShopScene::CreateShopEnts()
{
    const sf::Vector2f size = {170,170};
    int paddingX = 80;
    int paddingY = 200;
    int entries = 3;
    auto firstX = (Params::gameW - (size.x*entries + paddingX*(entries-1)))/2 + size.x/2;
    for (int i = 0; i < entries; i++)
    {
        buyButtons[i] = _entMan.CreateEntity();
        sf::Vector2f pos = {firstX + (paddingX + size.x)*i, Params::gameH - size.y/2 - paddingY};
        _entMan.add<Position>(buyButtons[i], {pos});
        sf::RectangleShape shape;
        shape.setSize({size.x,size.y});
        shape.setOrigin({size.x/2, size.y/2});
        _entMan.add<RectShape>(buyButtons[i], {1,shape});
        _entMan.add<Button>(buyButtons[i], {{size.x, size.y}, false, false});

        auto weapon = _entMan.CreateEntity();
        _entMan.add<Position>(weapon, {pos + sf::Vector2f(80,60)});
        sf::RectangleShape shape2;
        shape2.setSize({80,80});
        shape2.setOrigin({40,40});
        shape2.setFillColor(sf::Color::Magenta);
        _entMan.add<RectShape>(weapon, {2,shape2});
        _entMan.add<Button>(weapon, {{80,80}, false, false});
    }
}

void ShopScene::Update(const float& dt) 
{
    Scene::Update(dt);

    auto wallet = _entMan.get<WalletPtr>(_entMan.getAllEnt<WalletPtr>()[0]);
    _entMan.get<Text>(totalMoney)->txt.setString(std::to_string(wallet->walletPtr->money));

    auto butts = _entMan.getAllEnt<Button>();
    auto but = _entMan.get<Button>(butts[0]);
    auto rect = _entMan.get<RectShape>(butts[0]);

    for (auto cur : butts)
    {
        std::cout<<_entMan.get<Position>(cur)->pos.x<<" " << _entMan.get<Position>(cur)->pos.y << "\n";
    }
    std::cout<<MouseHelper::GetMousePos().x<<" "<<MouseHelper::GetMousePos().y<<"\n";
    std::cout<<"\n";

    rect->shape.setFillColor(sf::Color::White);
    if (but->pressed)
    {
        rect->shape.setFillColor(sf::Color::Green);
    }
    else if (but->hover)
    {
        rect->shape.setFillColor(sf::Color::Yellow);
    }

}