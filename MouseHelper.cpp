#include "MouseHelper.hpp"

void MouseHelper::SetWindow(sf::RenderWindow* rendWindow)
{
    window = rendWindow;
}

void MouseHelper::HandleEvents(sf::Event* event)
{
    if (event->type == sf::Event::MouseWheelScrolled)
    {
        mWheelMove = event->mouseWheelScroll.delta;
    }
    if (event->type == sf::Event::MouseButtonPressed)
    {
        pressed.insert(event->mouseButton.button);
    }
    if (event->type == sf::Event::MouseButtonReleased)
    {
        released.insert(event->mouseButton.button);
    }
}

sf::Vector2i MouseHelper::GetMousePos()
{
    return sf::Mouse::getPosition(*window);
}

bool MouseHelper::ButtonPressed(sf::Mouse::Button but)
{
    return pressed.contains(but);
}

bool MouseHelper::ButtonReleased(sf::Mouse::Button but)
{
    return released.contains(but);
}

float MouseHelper::MouseWheelMovement()
{
    return mWheelMove;
}

void MouseHelper::Clear()
{
    mWheelMove = 0;
    pressed.clear();
    released.clear();
}