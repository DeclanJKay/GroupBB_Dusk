#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

enum Screen
{
    safeHouse,
    towerDefence 
};

struct GameSys
{
    static void init();
    static void clean();
    static void update(const float &dt);
    static void render(sf::RenderWindow &window);
    static void SwitchPlayerRestrict(Screen scrn);
    static void ToggleGameScreen();
};