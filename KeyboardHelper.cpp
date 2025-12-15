#include "KeyboardHelper.hpp"

void KeyboardHelper::AssembleKeysPressed(sf::Event* event)
{
    if (event->type == sf::Event::KeyPressed)
    {
        keysPressed.insert(event->key.code);
    }
    else if (event->type == sf::Event::KeyReleased)
    {
        keysReleased.insert(event->key.code);
    }
}

bool KeyboardHelper::KeyPressed(sf::Keyboard::Key key)
{
    return keysPressed.contains(key);
}

bool KeyboardHelper::KeyReleased(sf::Keyboard::Key key)
{
    return keysReleased.contains(key);
}

void KeyboardHelper::Clear()
{
    keysReleased.clear();
    keysPressed.clear();
}