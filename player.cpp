#include "player.hpp"
#include "tile_level_loader/level_system.hpp"
#include "game_parameters.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include <cmath>

using ls = LevelSystem;
using param = Parameters;
// ------------------------------------------------------
// Constructor
// ------------------------------------------------------

Player::Player()
    : Entity(std::make_unique<sf::CircleShape>(kRadius)) {



    // Base colour for the player
    //_baseColor = sf::Color::Magenta;
    //_shape->setFillColor(_baseColor);

    // Centre the circle on its position
    _shape->setOrigin({ kRadius, kRadius });

    // Default start position (overwritten by scenes)
    set_position({ 100.f, 100.f });
}

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
            // Safehouse: free movement
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

    // Hit flash (runs every frame, not only while moving)
    if (_flashTimer > 0.f) {
        _flashTimer -= dt;
        if (_flashTimer < 0.f) _flashTimer = 0.f;

        // Simple white flash while timer is active
        _shape->setFillColor(sf::Color::White);
    }
    else {
        // Back to normal colour
        //_shape->setFillColor(_baseColor);
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

    // Base Entity hook (currently does nothing, but kept for consistency)
    Entity::update(dt);
}

void Player::render(sf::RenderWindow& window) const {
    window.draw(*_shape);
}

void Player::take_damage(int amount) {
    if (_health <= 0) return;  // already dead, ignore

    _health -= amount;
    if (_health < 0) _health = 0;

    // Start short hit flash
    _flashTimer = 0.2f;
}
//_______________________________________________
Player::Player()
    : Entity(std::make_unique<sf::CircleShape>(kRadius)) // dummy shape for base class
{
    // Load player texture (sprite sheet 256x64, 4 frames)
    if (!_texture.loadFromFile("res/img/playerAni.png")) {
        throw std::runtime_error("Failed to load player texture!");
    }

    _sprite.setTexture(_texture);

    // Scale if needed
    float scale = 1.0f;
    _sprite.setScale(scale, scale);

    // Center origin
    sf::FloatRect bounds = _sprite.getLocalBounds();
    _sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    // Start position
    set_position({ 100.f, 100.f });

    // Initial texture rectangle
    _sprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
}

// ------------------------------------------------------
// Position Support
// ------------------------------------------------------
sf::Vector2f Player::get_position() const {
    return _sprite.getPosition();
}

void Player::set_position(sf::Vector2f pos) {
    _sprite.setPosition(pos);
}

// ------------------------------------------------------
// Animation
// ------------------------------------------------------
void Player::update_animation(float dt) {
    // Only animate if any movement key is pressed
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        _animationTimer += dt;
        if (_animationTimer >= _frameDuration) {
            _animationTimer = 0.f;
            _currentFrame = (_currentFrame + 1) % _numFrames;
        }
    }
    else {
        _currentFrame = 0; // idle frame
    }

    // Update texture rectangle for current frame
    _sprite.setTextureRect(sf::IntRect(_currentFrame * 64, 0, 64, 64));

    // Keep origin centered
    sf::FloatRect bounds = _sprite.getLocalBounds();
    _sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
}

// ------------------------------------------------------
// Update logic
// ------------------------------------------------------
void Player::update(const float& dt) {
    sf::Vector2f dir{ 0.f, 0.f };

    // WASD + arrow movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) dir.x += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  dir.y += 1.f;

    // Movement calculation
    if (dir.x != 0.f || dir.y != 0.f) {
        const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        const sf::Vector2f norm = dir / len;
        const sf::Vector2f target = get_position() + norm * kSpeed * dt;

        if (!_use_tile_collision) {
            set_position(target);
        }
        else {
            try {
                const auto tile = ls::get_tile_at(target);
                if (tile != ls::WALL &&
                    tile != ls::WAYPOINT &&
                    tile != ls::ENEMY) {
                    set_position(target);
                }
            }
            catch (...) {
                set_position(target);
            }
        }
    }

    // Update animation
    update_animation(dt);

    // Hit flash
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


