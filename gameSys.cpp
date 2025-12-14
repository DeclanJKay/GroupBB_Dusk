#include <SFML/Graphics.hpp>
#include <iostream> //for debug
#include <cstdlib> //for rand
#include <ctime> //for seeding rand
#include <cmath> 
#include <string> //for score concat

#include "gameSys.hpp"
#include "gameParams.hpp"
#include "Systems.hpp"
#include "tile_level_loader/level_system.hpp"
#include "Scenes.hpp"
#include "KeyboardHelper.hpp"

using ls = LevelSystem;

SafeHouse shScene;
TowerDefence tdScene;
ShopScene shopScene;

Screen curScreen;
Screen lastScreen;

std::shared_ptr<Wallet> mainWallet;

void GameSys::init()
{
    mainWallet = std::make_shared<Wallet>();
    curScreen = safeHouse;
    
    shopScene = ShopScene(mainWallet);
    SwitchPlayerRestrict(curScreen);
}

void GameSys::update(const float &dt) 
{
    switch (curScreen)
    {
        case shop:
            shopScene.Update(dt, shScene.GetPlayerArsenal(), tdScene.GetTurretHand());
        case towerDefence:
        case safeHouse:
            shScene.Update(dt, tdScene.GetTransfers());
            tdScene.Update(dt, shScene.NoEnemies());
            ToggleGameScreen();
            break;
    }
}

void GameSys::render(sf::RenderWindow &window) 
{
    switch (curScreen)
    {
        case safeHouse:
            shScene.Draw(window);
            break;
        case towerDefence:
            ls::render(window);
            tdScene.Draw(window);
            break;
        case shop:
            shopScene.Draw(window);
            break;
    }
}

void GameSys::clean()
{
	
}

void GameSys::ToggleGameScreen()
{
    if (KeyboardHelper::KeyPressed(sf::Keyboard::Tab) && curScreen != shop)
    {
        if (curScreen == safeHouse)
            curScreen = towerDefence;
        else
            curScreen = safeHouse;
        SwitchPlayerRestrict(curScreen);
    }
    else if (KeyboardHelper::KeyPressed(sf::Keyboard::P))
    {
        if (curScreen != shop)
        {
            lastScreen = curScreen;
            curScreen = shop;
        }
        else
        {
            curScreen = lastScreen;
        }
    }
    else if (KeyboardHelper::KeyPressed(sf::Keyboard::Tab) || KeyboardHelper::KeyPressed(sf::Keyboard::Escape))
    {
        if (curScreen == shop)
        {
            curScreen = lastScreen;
        }
    }
    else {return;}
    SwitchPlayerRestrict(curScreen);
}

void GameSys::SwitchPlayerRestrict(Screen scrn)
{
    switch (scrn)
    {
        //the if statements only trigger if not initialised already
        case Screen::safeHouse:
            if (!shScene.SetRestrictPlayer(false)){ shScene = SafeHouse(mainWallet, false); }
            if(!tdScene.SetRestrictPlayer(true)){ tdScene = TowerDefence(mainWallet, true); }
            break;
        case Screen::towerDefence:
            if(!shScene.SetRestrictPlayer(true)){ shScene = SafeHouse(mainWallet, true); }
            if(!tdScene.SetRestrictPlayer(false)){ tdScene = TowerDefence(mainWallet, false); }
            break;
        case Screen::shop:
            if(!shScene.SetRestrictPlayer(true)){ shScene = SafeHouse(mainWallet, true); }
            if(!tdScene.SetRestrictPlayer(true)){ tdScene = TowerDefence(mainWallet, true); }
            break;
    }
}