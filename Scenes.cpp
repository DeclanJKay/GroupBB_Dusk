#include "Scenes.hpp"
#include "tile_level_loader/level_system.hpp"
#include "Comps.hpp"
#include "Upgrades.hpp"
#include <math.h>
#include "FileMgr.hpp"

using ls = LevelSystem;

void Scene::Update(const float& dt)
{
    _entMan.Update(dt);
}

void Scene::Draw(sf::RenderWindow& window)
{
    _entMan.Draw(window);
}

SafeHouse::SafeHouse(std::shared_ptr<Wallet> wallet,
                     std::shared_ptr<UpgradeData> upgrades,
                     bool playerRestrict)
{
    player = _entMan.CreatePlayer();

    //Test enemy
    auto type = EnemyTypes::eBasic;
    _entMan.CreateSHEnemy(&player, &type);

    // --- Shop entity ---
    auto shop = _entMan.CreateEntity();
    _entMan.add<Shop>(shop, Shop{});


    RestrictPlayerEnt = _entMan.CreateEntity();
    _entMan.add<RestrictPlayerInput>(RestrictPlayerEnt, {playerRestrict});

    // --- Upgrade UI text entity ---
    upgradeTextEnt = _entMan.CreateEntity();
    {
        sf::Text offerTxt;
        auto font = FileMgr::GetFont("res/fonts/arial.ttf");
        offerTxt.setFont(*font);
        offerTxt.setCharacterSize(20);
        offerTxt.setPosition(20.f, 20.f);
        offerTxt.setString("");
        _entMan.add<Text>(upgradeTextEnt, Text{offerTxt});
    }

    // --- Debug Stats text entity ---
    debugTextEnt = _entMan.CreateEntity();
    {
        sf::Text dbgTxt;
        auto font = FileMgr::GetFont("res/fonts/arial.ttf");
        dbgTxt.setFont(*font);
        dbgTxt.setCharacterSize(16);
        dbgTxt.setPosition(20.f, 170.f); // lower so it doesn’t overlap the upgrade list
        dbgTxt.setString("");
        _entMan.add<Text>(debugTextEnt, Text{dbgTxt});
    }

    
    if (wallet != nullptr)
    {
        auto wlt = _entMan.CreateEntity();
        _entMan.add<WalletPtr>(wlt, {wallet});
    }

    if (upgrades != nullptr)
    {
        auto upgEnt = _entMan.CreateEntity();
        _entMan.add<UpgradeDataPtr>(upgEnt, UpgradeDataPtr{upgrades});
    }
}


void SafeHouse::Update(const float& dt, std::vector<EnemyTypes> toSpawn)
{
    Scene::Update(dt);

    for (int i = 0; i < (int)toSpawn.size(); i++)
    {
        _entMan.CreateSHEnemy(&player, &toSpawn[i]);
    }

    // Toggle debug overlay
    if (KeyboardHelper::KeyPressed(sf::Keyboard::F3))
    {
        showDebug = !showDebug;
    }

    // ----- Upgrade offer text -----
    auto upgData = UpgradeManager::GetUpgradeData(_entMan);

    if (_entMan.Exists(upgradeTextEnt))
    {
        auto txtComp = _entMan.get<Text>(upgradeTextEnt);

        if (upgData && upgData->offerActive)
        {
            std::string s = "Choose an upgrade:\n";
            for (int i = 0; i < (int)upgData->offerOptions.size(); i++)
            {
                s += std::to_string(i + 1) + ") " +
                     UpgradeManager::UpgradeName(upgData->offerOptions[i]) + "\n";
            }
            s += "\nPress 1-3 to pick.";
            txtComp->txt.setString(s);
        }
        else
        {
            txtComp->txt.setString("");
        }
    }

    // ----- Debug overlay text -----
    if (_entMan.Exists(debugTextEnt))
    {
        auto dbg = _entMan.get<Text>(debugTextEnt);

        if (!showDebug)
        {
            dbg->txt.setString("");
        }
        else
        {
            std::string s;

            if (_entMan.has<Health>(player))
            {
                auto hp = _entMan.get<Health>(player);
                s += "HP: " + std::to_string(hp->hp) + "/" + std::to_string(hp->maxHealth) + "\n";
            }

            if (_entMan.has<PlayerMovement>(player))
            {
                auto mv = _entMan.get<PlayerMovement>(player);
                s += "MoveSpd: " + std::to_string(mv->moveSpd) + "\n";
            }

            if (_entMan.has<WeaponArsenal>(player))
            {
                auto ars = _entMan.get<WeaponArsenal>(player);
                int idx = ars->selected;

                if (idx >= 0 && idx < (int)ars->weapons.size())
                {
                    auto& w = ars->weapons[idx];
                    s += "Weapon[" + std::to_string(idx) + "] Dmg: " + std::to_string(w.damage) + "\n";
                    s += "Weapon[" + std::to_string(idx) + "] FR: " + std::to_string(w.fireRate) + "\n";
                    s += "Weapon[" + std::to_string(idx) + "] BulletSpd: " + std::to_string(w.bulletSpeed) + "\n";
                }
            }

            if (upgData != nullptr)
            {
                s += "\nUpgrades:\n";
                s += "bonusMaxHP: " + std::to_string(upgData->bonusMaxHP) + "\n";
                s += "bonusDamage: " + std::to_string(upgData->bonusDamage) + "\n";
                s += "bonusFireRate: " + std::to_string((int)(upgData->bonusFireRate * 100)) + " (x0.01)\n";
                s += "bonusMoveSpd: " + std::to_string(upgData->bonusMoveSpd) + "\n";
                s += "bonusBulletSpeed: " + std::to_string(upgData->bonusBulletSpeed) + "\n";
                s += "moneyBonus: " + std::to_string(upgData->moneyBonus) + "\n";
            }

            s += "\nF3: Toggle Debug";
            dbg->txt.setString(s);
        }
    }

    // Still handle key input (1-3) and apply upgrade
    UpgradeManager::HandleOfferInput(_entMan, player);
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

bool SafeHouse::ApplyUpgrade(UpgradeTypes upg)
{
    return UpgradeManager::ApplyUpgrade(_entMan, player, upg);
}



//TOWER DEFENCE
TowerDefence::TowerDefence(std::shared_ptr<Wallet> wallet, std::shared_ptr<UpgradeData> upgrades, bool playerRestrict)
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
    _entMan.add<Text>(testText, {txt});

    //create entity with reference to main wallet
    if (wallet == nullptr){return;}
    auto wlt = _entMan.CreateEntity();
    _entMan.add<WalletPtr>(wlt, {wallet});

    if (upgrades != nullptr)
    {
        auto upgEnt = _entMan.CreateEntity();
        _entMan.add<UpgradeDataPtr>(upgEnt, UpgradeDataPtr{upgrades});
    }
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

    // transfer reached-end enemies
    auto pathEnts = _entMan.getAllEnt<TDPathMove>();
    for (auto ent : pathEnts)
    {
        if (_entMan.get<TDPathMove>(ent)->reachedEnd)
        {
            toTransfer.push_back(_entMan.get<EnemyType>(ent)->type);
        }
    }

    auto spawners = _entMan.getAllEnt<WaveSpawner>();
    if (spawners.empty()) { return; }

    // "TD clear" means no active path enemies
    bool tdClear = _entMan.getAllEnt<TDPathMove>().empty();

    // "Wave finished spawning" means spawner budget is spent (or first wave not started yet)
    bool waveSpawnFinished = true;
    for (auto s : spawners)
    {
        auto sp = _entMan.get<WaveSpawner>(s);
        if (sp->lvlIndex >= 0 && sp->pointBudget > 0)
        {
            waveSpawnFinished = false;
            break;
        }
    }

    // Only between waves if BOTH scenes are clear AND TD is clear AND wave is finished spawning
    bool betweenWaves = allEnemiesDead && tdClear && waveSpawnFinished;

    if (betweenWaves)
    {
        UpgradeManager::TryTriggerOffer(_entMan, spawners);
    }

    // block starting waves while offer is active
    bool offerActive = false;
    auto upg = UpgradeManager::GetUpgradeData(_entMan);
    if (upg != nullptr) { offerActive = upg->offerActive; }

    for (auto s : spawners)
    {
        _entMan.get<WaveSpawner>(s)->canStart = (betweenWaves && !offerActive);
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