#include "player.hpp"
#include "tile_level_loader/level_system.hpp"
#include "game_parameters.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <stdexcept>
#include <cmath>

using ls = LevelSystem;
using param = Parameters;

// ------------------------------------------------------
// Constructor
// ------------------------------------------------------
Player::Player()
    : Entity(std::make_unique<sf::CircleShape>(kRadius))  // required by Entity
{
    // Load player texture (sprite sheet 256x64, 4 frames)
    if (!_texture.loadFromFile("res/img/playerAni.png")) {
        throw std::runtime_error("Failed to load player texture!");
    }

    _sprite.setTexture(_texture);

    // Optional scaling
    float scale = 1.0f;
    _sprite.setScale(scale, scale);

    // Center origin
    sf::FloatRect bounds = _sprite.getLocalBounds();
    _sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    // Default start position
    set_position({ 100.f, 100.f });

    // Initial animation frame
    _sprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
}

// ------------------------------------------------------
// Position helpers
// ------------------------------------------------------
sf::Vector2f Player::get_position() const {
    return _sprite.getPosition();
}

void Player::set_position(sf::Vector2f pos) {
    _sprite.setPosition(pos);
    _shape->setPosition(pos);   // keep dummy collider synced
}

// ------------------------------------------------------
// Animation
// ------------------------------------------------------
void Player::update_animation(float dt) {
    bool moving =
        sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down);

    if (moving) {
        _animationTimer += dt;
        if (_animationTimer >= _frameDuration) {
            _animationTimer = 0.f;
            _currentFrame = (_currentFrame + 1) % _numFrames;
        }
    }
    else {
        _currentFrame = 0; // idle frame
    }

    // Update texture frame
    _sprite.setTextureRect(sf::IntRect(_currentFrame * 64, 0, 64, 64));

    // Keep rotation origin correct
    sf::FloatRect bounds = _sprite.getLocalBounds();
    _sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
}

// ------------------------------------------------------
// Update logic
// ------------------------------------------------------
void Player::update(const float& dt) {
    sf::Vector2f dir{ 0.f, 0.f };

    // WASD + arrows
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) dir.x += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  dir.y += 1.f;

    // Movement
    if (dir.x != 0.f || dir.y != 0.f) {
        const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        sf::Vector2f norm = dir / len;

        const sf::Vector2f target = get_position() + norm * kSpeed * dt;

        if (!_use_tile_collision) {
            set_position(target);
        }
        else {
            try {
                const auto tile = ls::get_tile_at(target);
                if (tile != ls::WALL &&
                    tile != ls::WAYPOINT &&
                    tile != ls::ENEMY)
                {
                    set_position(target);
                }
            }
            catch (...) {
                set_position(target); // fallback
            }
        }
    }

    // Animation
    update_animation(dt);

    // Flash
    if (_flashTimer > 0.f) {
        _flashTimer -= dt;
        if (_flashTimer < 0.f) _flashTimer = 0.f;
        _sprite.setColor(sf::Color::White);
    }
    else {
        _sprite.setColor(sf::Color::Magenta);
    }

    // Clamp inside game window
    sf::Vector2f pos = get_position();
    pos.x = std::clamp(pos.x, kRadius, static_cast<float>(param::game_width) - kRadius);
    pos.y = std::clamp(pos.y, kRadius, static_cast<float>(param::game_height) - kRadius);
    set_position(pos);

    Entity::update(dt);
}

// ------------------------------------------------------
// Render
// ------------------------------------------------------
void Player::render(sf::RenderWindow& window) const {
    window.draw(_sprite);
}

// ------------------------------------------------------
// Damage
// ------------------------------------------------------
void Player::take_damage(int amount) {
    if (_health <= 0) return;

    _health -= amount;
    if (_health < 0) _health = 0;

    _flashTimer = 0.2f;
}

void Player::heal(int amount)
{
    if (amount <= 0) return;

    // Use your real internal HP variables here
    // Example if you have: int _health; int _maxHealth;
    _health += amount;
    if (_health > _maxHealth) {
        _health = _maxHealth;
    }
}

