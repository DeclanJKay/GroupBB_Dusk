#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_set>

class MouseHelper
{
    private:
        inline static sf::RenderWindow* window;
        inline static std::unordered_set<sf::Mouse::Button> pressed;
        inline static float mWheelMove;
    public:
        static void SetWindow(sf::RenderWindow* rendWindow);
        static sf::Vector2i GetMousePos();
        static void HandleEvents(sf::Event* event);
        static bool ButtonPressed(sf::Mouse::Button but);
        static float MouseWheelMovement();
        static void Clear();
};