#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_set>

class KeyboardHelper
{
    KeyboardHelper() = delete;
    ~KeyboardHelper() = delete;
    private:
        inline static std::unordered_set<sf::Keyboard::Key> keysPressed;
        inline static std::unordered_set<sf::Keyboard::Key> keysReleased;
    public:
        static bool KeyPressed(sf::Keyboard::Key key);
        static bool KeyReleased(sf::Keyboard::Key key);
        static void AssembleKeysPressed(sf::Event* event);
        static void Clear();
};