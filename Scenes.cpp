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

    RestrictPlayerEnt = _entMan.CreateEntity();
    _entMan.add<RestrictPlayerInput>(RestrictPlayerEnt, {playerRestrict});

    // --- Upgrade UI text entity ---
    upgradeTextEnt = _entMan.CreateEntity();
    {
        sf::Text offerTxt;
        auto font = FileMgr::GetFont("res/fonts/ARIAL.TTF");
        offerTxt.setFont(*font);
        offerTxt.setCharacterSize(20);
        offerTxt.setPosition(20.f, 20.f);
        offerTxt.setString("");
        _entMan.add<Text>(upgradeTextEnt, Text{5,offerTxt});
    }

    // --- Debug Stats text entity ---
    debugTextEnt = _entMan.CreateEntity();
    {
        sf::Text dbgTxt;
        auto font = FileMgr::GetFont("res/fonts/ARIAL.TTF");
        dbgTxt.setFont(*font);
        dbgTxt.setCharacterSize(16);
        dbgTxt.setPosition(20.f, 170.f); // lower so it doesn’t overlap the upgrade list
        dbgTxt.setString("");
        _entMan.add<Text>(debugTextEnt, Text{5, dbgTxt});
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


void SafeHouse::Update(const float& dt, std::vector<std::pair<EnemyTypes, int>> toSpawn)
{
    Scene::Update(dt);

    for (int i = 0; i < (int)toSpawn.size(); i++)
    {
        _entMan.CreateSHEnemy(&player, &toSpawn[i].first, toSpawn[i].second);
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
    return _entMan.getAllEnt<EnemyType>(true).empty();
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

WeaponArsenal* SafeHouse::GetPlayerArsenal()
{
    return _entMan.get<WeaponArsenal>(player);
}

bool SafeHouse::AllPlayersDead()
{
    return _entMan.getAllEnt<PlayerMovement>(true).empty();
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

    //spawner
    auto spawner = _entMan.CreateEntity();
    WaveSpawner spawnDef;
    spawnDef.canStart = true;
    spawnDef.iniPointBudget = 10;
    spawnDef.lvlIndex = -1; //lvl index gets increased after player starts wave, so set to -1 to begin at 0
    spawnDef.maxLvl = 0;
    spawnDef.path = sorted;
    spawnDef.pointIncrease = 20;
    spawnDef.spawnInterval = 1;
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

    if (upgrades != nullptr)
    {
        auto upgEnt = _entMan.CreateEntity();
        _entMan.add<UpgradeDataPtr>(upgEnt, UpgradeDataPtr{upgrades});
    }

    //create turret handler
    turretHand = _entMan.CreateEntity();
    _entMan.add<TurretHandler>(turretHand, {{{Turrets::tBasic, 2}}, 0, 0.3f});
    _entMan.add<Position>(turretHand, {{30, 30}});
    sf::RectangleShape shape;
    shape.setFillColor(sf::Color::Black);
    _entMan.add<RectShape>(turretHand, {20, shape});
    sf::Text txt;
    txt.setFont(*FileMgr::GetFont("res/fonts/ARIAL.TTF"));
    txt.setFillColor(sf::Color::White);
    txt.setCharacterSize(20);
    _entMan.add<Text>(turretHand, {21, txt});
    _entMan.add<Dragable>(turretHand, {});
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
            auto hp = _entMan.get<Health>(ent)->hp;
            if (_entMan.has<Shield>(ent))
                hp += std::max(_entMan.get<Shield>(ent)->amount, 0);
            toTransfer.push_back({_entMan.get<EnemyType>(ent)->type, hp});
        }
    }

    auto spawners = _entMan.getAllEnt<WaveSpawner>();
    if (spawners.empty()) { return; }

    // "TD clear" means no active path enemies
    bool tdClear = _entMan.getAllEnt<TDPathMove>(true).empty();

    // "Wave finished spawning" means spawner budget is spent (or first wave not started yet)
    bool waveSpawnFinished = true;
    auto noUpgrades = false;
    for (auto s : spawners)
    {
        auto sp = _entMan.get<WaveSpawner>(s);
        if (sp->pointBudget > 0) {waveSpawnFinished=false;}
        if (sp->lvlIndex == -1 || sp->lvlIndex == sp->maxLvl) {noUpgrades=true;}
    }

    // Only between waves if BOTH scenes are clear AND TD is clear AND wave is finished spawning
    bool betweenWaves = allEnemiesDead && tdClear && waveSpawnFinished;

    if (betweenWaves && !noUpgrades)
    {
        UpgradeManager::TryTriggerOffer(_entMan, spawners);
    }

    // block starting waves while offer is active
    bool offerActive = false;
    auto upg = UpgradeManager::GetUpgradeData(_entMan);
    if (upg != nullptr) { offerActive = upg->offerActive; }
    if (offerActive) {return;}

    if (!betweenWaves)
        return;
    for (auto s : spawners)
    {
        _entMan.get<WaveSpawner>(s)->canStart = true;
    }
}


bool TowerDefence::SetRestrictPlayer(bool b)
{
    if (!_entMan.Exists(RestrictPlayerEnt)) {return false;}
    _entMan.get<RestrictPlayerInput>(RestrictPlayerEnt)->restrict = b;
    return true;
}

std::vector<std::pair<EnemyTypes, int>> TowerDefence::GetTransfers()
{
    auto returnable = toTransfer;
    toTransfer.clear();
    return returnable;
}

TurretHandler* TowerDefence::GetTurretHand()
{
    return _entMan.get<TurretHandler>(turretHand);
}

bool TowerDefence::HasEnded()
{
    for (auto spawner : _entMan.getAllEnt<WaveSpawner>())
    {
        auto curSpawn = _entMan.get<WaveSpawner>(spawner);
        if (curSpawn->lvlIndex < curSpawn->maxLvl){return false;}
        if (!curSpawn->canStart) {return false;}
    }
    return true;
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

    _entMan.PopulateShop(shop);

    //create money display
    totalMoney = _entMan.CreateEntity();
    sf::Text txt;
    txt.setFont(*FileMgr::GetFont("res/fonts/ARIAL.TTF"));
    txt.setString(std::to_string(wallet->money));
    _entMan.add<Text>(totalMoney, {1,txt});

    InitialiseShopInterface(shop);
    CreateHoverDescription();
}

void ShopScene::CreateHoverDescription()
{
    hoverDesc = _entMan.CreateEntity();
    _entMan.add<Position>(hoverDesc, {});
    sf::RectangleShape shape;
    shape.setFillColor(sf::Color::Yellow);
    _entMan.add<RectShape>(hoverDesc, {3, shape});
    sf::Text txt;
    txt.setFillColor(sf::Color::Black);
    txt.setFont(*FileMgr::GetFont("res/fonts/ARIAL.TTF"));
    _entMan.add<Text>(hoverDesc, {4, txt});
    _entMan.Disable(hoverDesc);
}

void ShopScene::UpdateHoverDesc(std::string text)
{
    int padding = 10;
    _entMan.Enable(hoverDesc);
    auto pos = _entMan.get<Position>(hoverDesc);
    pos->pos = (sf::Vector2f)MouseHelper::GetMousePos();
    auto txt = _entMan.get<Text>(hoverDesc);
    txt->txt.setString(text);
    txt->txt.setOrigin({-padding/2,-padding/2});
    auto rect = _entMan.get<RectShape>(hoverDesc);
    auto txtSize = txt->txt.getGlobalBounds();
    rect->shape.setSize({txtSize.width + 10, txtSize.height + 10});
    auto sticksout = Params::gameW - (rect->shape.getSize().x + pos->pos.x);
    if (sticksout < 0)
    {
        pos->pos.x += sticksout;
    }
}

void ShopScene::ShowDesc(Turrets turret)
{
    auto stats = TurretStatsManager::GetStats(turret);
    std::string text = 
        TurretStatsManager::GetTurretName(turret) + "\n" + 
        "Range: " + std::to_string(stats.range)  + "\n" + 
        "Damage: " + std::to_string(stats.weapons.weapons[0].damage)  + "\n" + 
        "BulletsShot: " + std::to_string(stats.weapons.weapons[0].bulletsShot)  + "\n" + 
        "Firerate: " + std::to_string(stats.weapons.weapons[0].fireRate)  + "\n" + 
        "Spread: " + std::to_string(stats.weapons.weapons[0].bulletSpread)
    ;
    UpdateHoverDesc(text);
}

void ShopScene::ShowDesc(Weapons weapon)
{
    auto stats = WeaponStatsMgr::GetStats(weapon);
    std::string text = 
        WeaponStatsMgr::GetWeaponName(weapon) + "\n" + 
        "Range: " + std::to_string(stats.bulletLifetime * stats.bulletSpeed)  + "\n" + 
        "Damage: " + std::to_string(stats.damage)  + "\n" + 
        "BulletsShot: " + std::to_string(stats.bulletsShot)  + "\n" + 
        "Firerate: " + std::to_string(stats.fireRate)  + "\n" + 
        "Spread: " + std::to_string(stats.bulletSpread)
        ;
    UpdateHoverDesc(text);
}

void ShopScene::InitialiseShopInterface(Entity shop)
{
    const sf::Vector2f size = {170,170};
    int paddingX = 80;
    int paddingY = 200;
    int entries = 3;
    auto firstX = (Params::gameW - (size.x*entries + paddingX*(entries-1)))/2 + size.x/2;

    auto ordered = _entMan.get<Shop>(shop, true)->order; //is just a copy of the data

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
        _entMan.add<ChangeButCol>(buyButtons[i], {sf::Color::White, {180,180,180,255}, {150,150,150,255}});

        //replace this with a sprite later
        sf::Text txt2;
        txt2.setFont(*FileMgr::GetFont("res/fonts/ARIAL.TTF"));
        txt2.setString(TurretStatsManager::GetTurretName(ordered[i].first));
        txt2.setColor(sf::Color::Black);
        txt2.setOrigin(txt2.getGlobalBounds().getSize()/2.f);
        _entMan.add<Text>(buyButtons[i], {2, txt2});

        weaponButts[i] = _entMan.CreateEntity();
        _entMan.add<Position>(weaponButts[i], {pos + sf::Vector2f(80,60)});
        sf::RectangleShape shape2;
        shape2.setSize({80,80});
        shape2.setOrigin({40,40});
        shape2.setFillColor(sf::Color::Magenta);
        _entMan.add<RectShape>(weaponButts[i], {2,shape2});
        _entMan.add<Button>(weaponButts[i], {{80,80}, false, false});

        //replace this with a sprite later
        sf::Text txt;
        txt.setFont(*FileMgr::GetFont("res/fonts/ARIAL.TTF"));
        txt.setString(WeaponStatsMgr::GetWeaponName(ordered[i].second));
        txt.setColor(sf::Color::Black);
        txt.setCharacterSize(20);
        txt.setOrigin(txt.getGlobalBounds().getSize()/2.f);
        _entMan.add<Text>(weaponButts[i], {2, txt});

        //prices
        sf::Text price;
        price.setFont(*FileMgr::GetFont("res/fonts/ARIAL.TTF"));
        prices[i] = _entMan.CreateEntity();
        _entMan.add<Position>(prices[i] , {{pos.x, pos.y + size.y/2 + 50}});
        ChangeStringCentred(price, std::to_string(_entMan.get<Shop>(shop, true)->prices[i]));
        _entMan.add<Text>(prices[i] , {1, price});
    }
}

void ShopScene::UpdateShopEnt(int i)
{
    auto shop = _entMan.get<Shop>(_entMan.getAllEnt<Shop>()[0]);
    auto butTxt = _entMan.get<Text>(buyButtons[i]);
    auto weaponTxt = _entMan.get<Text>(weaponButts[i]);
    auto priceTxt = _entMan.get<Text>(prices[i]);

    //update but
    ChangeStringCentred(butTxt->txt, TurretStatsManager::GetTurretName(shop->order[i].first));

    //update weapons
    ChangeStringCentred(weaponTxt->txt, WeaponStatsMgr::GetWeaponName(shop->order[i].second));

    //update price
    ChangeStringCentred(priceTxt->txt, std::to_string(shop->prices[i]));
    
    
    
    //butTxt->txt.setString(std::to_string(shop->prices[i]));
    //butTxt->txt.setOrigin(butTxt->txt.getGlobalBounds().getSize()/2.f);
}

void ShopScene::UpdatePrices()
{
    for (int i = 0; i < 3; i++)
    {
        auto txt = _entMan.get<Text>(prices[i]);
        auto shop = _entMan.get<Shop>(_entMan.getAllEnt<Shop>()[0]);

        if (shop->prices[i] <= _entMan.get<WalletPtr>(_entMan.getAllEnt<WalletPtr>()[0])->walletPtr->money)
        {
            txt->txt.setColor(sf::Color::White);
            continue;
        }
        txt->txt.setColor(sf::Color::Red);
    }
}

void ShopScene::Update(const float& dt, WeaponArsenal* ars, TurretHandler* turHand) 
{
    Scene::Update(dt);

    _entMan.Disable(hoverDesc);
    UpdatePrices();

    auto wallet = _entMan.get<WalletPtr>(_entMan.getAllEnt<WalletPtr>()[0]);
    _entMan.get<Text>(totalMoney)->txt.setString(std::to_string(wallet->walletPtr->money));

    auto shop = _entMan.get<Shop>(_entMan.getAllEnt<Shop>()[0]);

    //weapon hover desc
    for (int i = 0; i < 3; i++)
    {
        auto but = _entMan.get<Button>(weaponButts[i]);
        if (but->hover)
        {
            ShowDesc(shop->order[i].second);
            return;
        }
    }

    //turret hover desk
    for (int i = 0; i < 3; i++)
    {
        auto but = _entMan.get<Button>(buyButtons[i]);
        if (but->pressed)
        {
            if (_entMan.BuyFromShop(i, _entMan.getAllEnt<Shop>()[0], ars, turHand))
            {
                UpdateShopEnt(i);
            }
        }
        
        if (but->hover)
        {
            auto shop = _entMan.get<Shop>(_entMan.getAllEnt<Shop>()[0]);
            ShowDesc(shop->order[i].first);
            return;
        }
    }
}


GameOver::GameOver()
{
    int x = Params::gameW/2;
    //status txt
    statusTxt = _entMan.CreateEntity();
    _entMan.add<Position>(statusTxt, {{x,100}});
    sf::Text txt;
    txt.setFont(*FileMgr::GetFont("res/fonts/ARIAL.TTF"));
    txt.setFillColor(sf::Color::White);
    _entMan.add<Text>(statusTxt, {1,txt});

    //brokie txt
    brokieText = _entMan.CreateEntity();
    _entMan.add<Position>(brokieText, {{x,200}});
    sf::Text txt2;
    txt2.setFont(*FileMgr::GetFont("res/fonts/ARIAL.TTF"));
    txt2.setFillColor(sf::Color::White);
    txt2.setCharacterSize(20);
    _entMan.add<Text>(brokieText, {1,txt2});

    //main menu button
    menuBut = _entMan.CreateButton(
        {x,400},
        {300,100},
        "Main Menu",
        {sf::Color::White, {180,180,180,255}, {150,150,150,255}},
        sf::Color::Black
    );
}

bool GameOver::GoToMainMenu()
{
    return _entMan.get<Button>(menuBut)->pressed;
}

void GameOver::SetWin(bool status, int money)
{
    auto sTxt = _entMan.get<Text>(statusTxt, true);
    auto bTxt = _entMan.get<Text>(brokieText, true);

    switch (status)
    {
        case true:
            sTxt->txt.setFillColor(sf::Color::Green);
            sTxt->txt.setString("You have prevailed me laddie");
            if (money > 50)
                bTxt->txt.setString("And with great fortune too!");
            else
                bTxt->txt.setString("Could have saved some money though");
            break;
        case false:
            sTxt->txt.setFillColor(sf::Color::Red);
            sTxt->txt.setString("Embarassing");
            if (money > 50)
                bTxt->txt.setString("Nice stash");
            else
                bTxt->txt.setString("Didn't even leave behind a worthwile inheritence");
            break;
    }
    
    sTxt->txt.setOrigin(sTxt->txt.getGlobalBounds().getSize()/2.f);
    bTxt->txt.setOrigin(bTxt->txt.getGlobalBounds().getSize()/2.f);
    _entMan.get<Position>(brokieText, true)->pos.y = _entMan.get<Position>(statusTxt, true)->pos.y + sTxt->txt.getOrigin().y+bTxt->txt.getOrigin().y+30;
}