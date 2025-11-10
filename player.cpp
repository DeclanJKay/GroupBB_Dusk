#include "player.hpp"
#include "tile_level_loader/level_system.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <cmath>

using ls = LevelSystem;


Player::Player()
    : Entity(std::make_unique<sf::CircleShape>(kRadius)) {
    _shape->setFillColor(sf::Color::Magenta);
    _shape->setOrigin({ kRadius, kRadius });
    set_position({ 100.f, 100.f }); // MazeScene::reset will overwrite to START
}

void Player::update(const float& dt) {
    sf::Vector2f dir{ 0.f, 0.f };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) dir.x += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  dir.y += 1.f;

    if (dir.x != 0.f || dir.y != 0.f) {
        const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        dir /= len;
        const sf::Vector2f target = get_position() + dir * kSpeed * dt;

        // Block walls
        try {
            if (ls::get_tile_at(target) != ls::WALL) {
                set_position(target);
            }
        }
        catch (...) {
            // out of bounds—ignore movement
        }
    }

    Entity::update(dt);
}

void Player::render(sf::RenderWindow& window) const {
    window.draw(*_shape);
}
