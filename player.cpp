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

    // This is just a default; MazeScene / Safehouse will overwrite it
    set_position({ 100.f, 100.f });
}

void Player::update(const float& dt) {
    sf::Vector2f dir{ 0.f, 0.f };

    // Basic WASD / Arrow movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) dir.x += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  dir.y += 1.f;

    if (dir.x != 0.f || dir.y != 0.f) {
        // Normalise direction
        const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        sf::Vector2f norm = dir / len;

        const sf::Vector2f target = get_position() + norm * kSpeed * dt;

        if (!_use_tile_collision) {
            // Safehouse: no tile collisions, move freely
            set_position(target);
        }
        else {
            // Maze / Tower Defence: collide with walls / path tiles
            try {
                const auto tile = ls::get_tile_at(target);

                if (tile != ls::WALL &&
                    tile != ls::WAYPOINT &&   // enemy lane
                    tile != ls::ENEMY) {      // reserved tile type
                    set_position(target);
                }
            }
            catch (...) {
                // If level system throws (no level / out of range),
                // just allow movement.
                set_position(target);
            }
        }
    }

    // Let base Entity do any extra per-frame work
    Entity::update(dt);
}

void Player::render(sf::RenderWindow& window) const {
    window.draw(*_shape);
}

void Player::take_damage(int amount) {
    _health -= amount;
    if (_health < 0) {
        _health = 0;
    }

    // Later we can add:
    // - brief colour flash
    // - knockback
    // - death handling (game over, restart, etc.)
}
