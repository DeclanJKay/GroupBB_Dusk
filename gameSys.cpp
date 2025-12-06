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

using ls = LevelSystem;

enum Screen
{
    safeHouse,
    towerDefence 
};

SafeHouse shScene;
TowerDefence tdScene;

Screen curScreen;

void GameSys::init()
{
    shScene = SafeHouse();
    tdScene = TowerDefence();
    
    curScreen = safeHouse;
}

void GameSys::update(const float &dt) 
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
    {
        curScreen = towerDefence;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
    {
        curScreen = safeHouse;
    }
    switch (curScreen)
    {
        case towerDefence:
            shScene.Update(dt, tdScene.GetTransfers());
            tdScene.Update(dt);
            break;
        case safeHouse:
            shScene.Update(dt, tdScene.GetTransfers());
            tdScene.Update(dt);
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