#pragma once

#include "entity.hpp"
#include <SFML/Graphics.hpp>

// Simple player controlled circle
class Player : public Entity {
public:
    Player();

    // Per–frame logic (input, movement, flash, clamping)
    void update(const float& dt) override;

    // Draw the player to the window
    void render(sf::RenderWindow& window) const override;

    // Turn tile-based collision on/off
    // (true in Maze / Tower Defence, false in Safehouse)
    void set_use_tile_collision(bool enabled) { _use_tile_collision = enabled; }

    // --- Health API ---
    int  get_health() const { return _health; }
    int  get_max_health() const { return _maxHealth; }
    bool is_dead() const { return _health <= 0; }

    // Apply damage and trigger hit flash
    void take_damage(int amount);

    // Radius used for simple circle collision checks
    float get_radius() const { return kRadius; }

private:
    static constexpr float kRadius = 25.f;       // visual + collision radius
    static constexpr float kSpeed = 200.f;      // movement speed (units/sec)

    bool _use_tile_collision = false;            // if true, respect level tiles

    // Health
    int _maxHealth = 5;
    int _health = _maxHealth;

    float     _flashTimer = 0.f;                // seconds left of hit flash
    sf::Color _baseColor;                       // normal player colour
};
//----------------------------------------------
// Player controlled sprite character with animation
class Player : public Entity {
public:
    Player();

    // -----------------------------------------
    // Sprite + texture (visual representation)
    // -----------------------------------------
    sf::Texture _texture;
    sf::Sprite  _sprite;

    // Position helpers
    sf::Vector2f get_position() const;
    void set_position(sf::Vector2f pos);

    // Per-frame logic (movement, input, animation, flash, clamping)
    void update(const float& dt) override;

    // Render the player
    void render(sf::RenderWindow& window) const override;

    // Enable or disable tile-based collision
    void set_use_tile_collision(bool enabled) { _use_tile_collision = enabled; }

    // --- Health API ---
    int  get_health() const { return _health; }
    int  get_max_health() const { return _maxHealth; }
    bool is_dead() const { return _health <= 0; }

    // Apply damage and trigger hit flash
    void take_damage(int amount);

    // Approximate radius for collision & clamping
    float get_radius() const { return kRadius; }

private:
    // -----------------------------------------
    // Movement
    // -----------------------------------------
    static constexpr float kRadius = 25.f;  // collision boundary
    static constexpr float kSpeed = 200.f;  // movement speed (units/sec)
    bool _use_tile_collision = false;

    // -----------------------------------------
    // Health system
    // -----------------------------------------
    int _maxHealth = 5;
    int _health = _maxHealth;
    float _flashTimer = 0.f; // hit flash timer

    // -----------------------------------------
    // Animation
    // -----------------------------------------
    int   _currentFrame = 0;       // current frame index
    float _animationTimer = 0.f;   // time accumulator
    float _frameDuration = 0.15f;  // seconds per frame
    int   _numFrames = 4;          // total frames in sprite sheet

    // Update texture rectangle for animation
    void update_animation(float dt);
};

