#pragma once

#include "entity.hpp"
#include <SFML/Graphics.hpp>

// Player controlled sprite character with animation & rotation
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

    // Per-frame logic (movement, animation, rotation, flash, clamping)
    void update(const float& dt) override;

    // Render player on screen
    void render(sf::RenderWindow& window) const override;

    // Enable/disable tile-based collision (used in TD/Maze)
    void set_use_tile_collision(bool enabled) { _use_tile_collision = enabled; }

    // --- Health API ---
    int  get_health() const { return _health; }
    int  get_max_health() const { return _maxHealth; }
    bool is_dead() const { return _health <= 0; }
    void heal(int amount);

    // Damage + flash effect
    void take_damage(int amount);

    // Collision boundary radius
    float get_radius() const { return kRadius; }

private:
    // -----------------------------------------
    // Movement
    // -----------------------------------------
    static constexpr float kRadius = 25.f;   // collision & clamping radius
    static constexpr float kSpeed = 200.f;  // movement speed
    bool _use_tile_collision = false;

    // -----------------------------------------
    // Health system
    // -----------------------------------------
    int _maxHealth = 5;
    int _health = _maxHealth;
    float _flashTimer = 0.f;

    // -----------------------------------------
    // Animation (sprite sheet)
    // -----------------------------------------
    int   _currentFrame = 0;
    float _animationTimer = 0.f;
    float _frameDuration = 0.15f;
    int   _numFrames = 4;

    void update_animation(float dt);
    void update_rotation();     // faces mouse cursor
};

//----------------------------------------------
