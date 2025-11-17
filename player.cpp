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
        sf::Vector2f normDir = dir / len;

        const sf::Vector2f target = get_position() + normDir * kSpeed * dt;

        // Safehouse: free movement (no tile collision)
        if (!_use_tile_collision) {
            set_position(target);
        }
        else {
            try {
                // If a level is loaded: block walls *and* lane tiles
                const auto tile = ls::get_tile_at(target);

                if (tile != ls::WALL &&
                    tile != ls::WAYPOINT &&   // enemy lane is blocked
                    tile != ls::ENEMY) {      // reserved for future
                    set_position(target);
                }
            }
            catch (...) {
                // No level / out of range -> allow free movement
                // (used in scenes that don't rely on tiles safely)
                set_position(target);
            }
        }
    }

    Entity::update(dt);
}

void Player::render(sf::RenderWindow& window) const {
    window.draw(*_shape);
}
