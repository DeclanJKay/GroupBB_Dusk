#include "KeyboardHelper.hpp"

std::unordered_set<sf::Keyboard::Key> KeyboardHelper::keysPressed;

void KeyboardHelper::AssembleKeysPressed(sf::Event* event)
{
    if (event->type == sf::Event::KeyPressed)
    {
        keysPressed.insert(event->key.code);
    }
}

bool KeyboardHelper::KeyPressed(sf::Keyboard::Key key)
{
    return keysPressed.contains(key);
}

void KeyboardHelper::Clear()
{
    keysPressed.clear();
}