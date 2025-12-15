#include "player.hpp"
#include "tile_level_loader/level_system.hpp"
#include "game_parameters.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <stdexcept>
#include <cmath>

using ls = LevelSystem;
using param = Parameters;

// ------------------------------------------------------
// Constructor
// ------------------------------------------------------
Player::Player()
    : Entity(std::make_unique<sf::CircleShape>(kRadius)) // satisfy Entity Actor
{
    // Load player sprite sheet (expecting 256x64 with 4x 64x64 frames)
    if (!_texture.loadFromFile("res/img/Test.png")) {
        throw std::runtime_error("Failed to load player texture: res/img/Test.png");
    }

    _sprite.setTexture(_texture);

    // Keep default scale (tweak if needed)
    _sprite.setScale(1.f, 1.f);

    // Center origin for correct rotation/pivot
    sf::FloatRect bounds = _sprite.getLocalBounds();
    _sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    // Default start position
    set_position({ 100.f, 100.f });

    // Start on first frame
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

    // Keep dummy base shape (if Entity exposes _shape) in sync so collision/other code depending on it still works.
    // This assumes Entity provides a protected _shape pointer (as in your earlier code).
    if (_shape) {
        _shape->setPosition(pos);
    }
}

// ------------------------------------------------------
// Animation helper
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

    // Set texture rectangle to current frame
    _sprite.setTextureRect(sf::IntRect(_currentFrame * 64, 0, 64, 64));

    // Re-center origin (keeps consistent if texture rect changed)
    sf::FloatRect bounds = _sprite.getLocalBounds();
    _sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
}

// ------------------------------------------------------
// Update
// ------------------------------------------------------
void Player::update(const float& dt) {
    sf::Vector2f dir{ 0.f, 0.f };

    // Basic WASD / Arrow movement input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) dir.x += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  dir.y += 1.f;

    if (dir.x != 0.f || dir.y != 0.f) {
        // Normalise direction so diagonal speed isn’t faster
        const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        sf::Vector2f norm = dir / len;

        const sf::Vector2f target = get_position() + norm * kSpeed * dt;

        if (!_use_tile_collision) {
            // Free movement
            set_position(target);
        }
        else {
            // Maze / Tower Defence: respect level collision
            try {
                const auto tile = ls::get_tile_at(target);

                if (tile != ls::WALL &&
                    tile != ls::WAYPOINT &&   // enemy lane
                    tile != ls::ENEMY) {      // reserved tile type
                    set_position(target);
                }
            }
            catch (...) {
                // If level system fails (out of bounds etc.), just allow movement
                set_position(target);
            }
        }
    }

    // Update animation state
    update_animation(dt);

    // Hit flash (sprite tint)
    if (_flashTimer > 0.f) {
        _flashTimer -= dt;
        if (_flashTimer < 0.f) _flashTimer = 0.f;

        _sprite.setColor(sf::Color::Red);
    }
    else {
        _sprite.setColor(sf::Color::White);
    }

    // Clamp player inside the game window
    sf::Vector2f pos = get_position();

    const float minX = kRadius;
    const float maxX = static_cast<float>(param::game_width) - kRadius;
    const float minY = kRadius;
    const float maxY = static_cast<float>(param::game_height) - kRadius;

    pos.x = std::clamp(pos.x, minX, maxX);
    pos.y = std::clamp(pos.y, minY, maxY);

    set_position(pos);

    // Base Entity hook
    Entity::update(dt);
}

// ------------------------------------------------------
// Render (rotate to face cursor, then draw)
// ------------------------------------------------------
void Player::render(sf::RenderWindow& window) const {
    // Compute mouse world position and rotate sprite to face it.
    // render() is const in the Scene API, so we temporarily remove const on _sprite to set rotation.
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);

    sf::Vector2f pos = _sprite.getPosition();
    sf::Vector2f diff = mouseWorld - pos;

    float angleDeg = std::atan2(diff.y, diff.x) * 180.f / 3.14159265f;

    // Many top-down sprites face "up", so add +90 to align properly.
    // Adjust if your art faces a different direction.
    const_cast<sf::Sprite&>(_sprite).setRotation(angleDeg + 90.f);

    // Draw sprite
    window.draw(_sprite);
}

// ------------------------------------------------------
// Damage
// ------------------------------------------------------
void Player::take_damage(int amount) {
    if (_health <= 0) return;  // already dead, ignore

    _health -= amount;
    if (_health < 0) _health = 0;

    // Start short hit flash
    _flashTimer = 0.2f;
}
