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
GameOver gameOverScene;
MainMenu menuScene;
InstructScene instScene;

Screen curScreen;
Screen lastScreen;

std::shared_ptr<Wallet> mainWallet;
std::shared_ptr<UpgradeData> mainUpgrades;

void GameSys::init()
{
    mainWallet = std::make_shared<Wallet>();
    mainUpgrades = std::make_shared<UpgradeData>();
    curScreen = mainMenu;
    
    shopScene = ShopScene(mainWallet);
    SwitchPlayerRestrict(curScreen);
}

void GameSys::update(const float &dt) 
{
    switch (curScreen)
    {
        case Screen::shop:
            shopScene.Update(dt, shScene.GetPlayerArsenal(), tdScene.GetTurretHand());
            if (shopScene.Heal())
                shScene.HealPlayer(1); //todo: get rid of the hardcoded shite
        case Screen::towerDefence:
        case Screen::safeHouse:
            shScene.Update(dt, tdScene.GetTransfers());
            tdScene.Update(dt, shScene.NoEnemies());

            //check if gameover, and switch scene if so
            if (tdScene.HasEnded())
            {
                curScreen = gameOver;
                gameOverScene.SetWin(true, mainWallet->money);
            }
            else if (shScene.AllPlayersDead())
            {
                curScreen = gameOver;
                gameOverScene.SetWin(false, mainWallet->money);
            }

            ToggleGameScreen();
            break;
        case Screen::gameOver:
            gameOverScene.Update(dt);
            if (gameOverScene.GoToMainMenu())
                curScreen = mainMenu;
            break;
        case Screen::mainMenu:
            menuScene.Update(dt);
            if (menuScene.StartGame())
                curScreen = Screen::Instructions;
            break;
        case Screen::Instructions:
            instScene.Update(dt);
            if (instScene.StartGame())
            {
                curScreen = Screen::safeHouse;
                shScene = SafeHouse();
                tdScene = TowerDefence();
                SwitchPlayerRestrict(curScreen);
            }
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
        case Screen::gameOver:
            gameOverScene.Draw(window);
            break;
        case Screen::mainMenu:
            menuScene.Draw(window);
            if (menuScene.ExitGame())
                window.close();
            break;
        case Screen::Instructions:
            instScene.Draw(window);
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
        case Screen::safeHouse:
            if (!shScene.SetRestrictPlayer(false)) { 
                shScene = SafeHouse(mainWallet, mainUpgrades, false); 
            }
            if (!tdScene.SetRestrictPlayer(true))  { tdScene = TowerDefence(mainWallet, mainUpgrades, true);  }
            break;

        case Screen::towerDefence:
            if (!shScene.SetRestrictPlayer(true))  { shScene = SafeHouse(mainWallet, mainUpgrades, true);  }
            if (!tdScene.SetRestrictPlayer(false)) { tdScene = TowerDefence(mainWallet, mainUpgrades, false); }
            break;
        case Screen::shop:
            if(!shScene.SetRestrictPlayer(true)){ shScene = SafeHouse(mainWallet, mainUpgrades, true); }
            if(!tdScene.SetRestrictPlayer(true)){ tdScene = TowerDefence(mainWallet, mainUpgrades, true); }
            break;
    }
}
