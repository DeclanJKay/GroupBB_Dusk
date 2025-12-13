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

Screen curScreen;

std::shared_ptr<Wallet> mainWallet;

void GameSys::init()
{
    mainWallet = std::make_shared<Wallet>();
    curScreen = safeHouse;
    SwitchPlayerRestrict(curScreen);
}

void GameSys::update(const float &dt) 
{
    switch (curScreen)
    {
        case towerDefence: //implementation for 'case towerDefence || safeHouse:'
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
    }
}

void GameSys::clean()
{
	
}

void GameSys::ToggleGameScreen()
{
    if (KeyboardHelper::KeyPressed(sf::Keyboard::Tab))
    {
        if (curScreen == safeHouse)
            curScreen = towerDefence;
        else
            curScreen = safeHouse;
        SwitchPlayerRestrict(curScreen);
    }
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
    }
}