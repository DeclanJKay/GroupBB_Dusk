#pragma once

#include "entity.hpp"
#include <SFML/Graphics.hpp>

// Player controlled sprite character with animation & mouse-facing rotation
class Player : public Entity {
public:
    Player();

    // Sprite + texture
    sf::Texture _texture;
    sf::Sprite  _sprite;

    // Position helpers
    sf::Vector2f get_position() const;
    void set_position(sf::Vector2f pos);

    // Per-frame logic (movement, animation, flash, clamping)
    void update(const float& dt) override;

    // Render the player (rotates to face mouse here)
    void render(sf::RenderWindow& window) const override;

    // Toggle tile-based collision
    void set_use_tile_collision(bool enabled) { _use_tile_collision = enabled; }

    // Health API
    int  get_health() const { return _health; }
    int  get_max_health() const { return _maxHealth; }
    bool is_dead() const { return _health <= 0; }

    // Damage
    void take_damage(int amount);

    // Collision radius
    float get_radius() const { return kRadius; }

private:
    // Movement
    static constexpr float kRadius = 25.f;
    static constexpr float kSpeed = 200.f;
    bool _use_tile_collision = false;

    // Health
    int _maxHealth = 5;
    int _health = _maxHealth;
    float _flashTimer = 0.f;

    // Animation (4-frame horizontal sheet)
    int   _currentFrame = 0;
    float _animationTimer = 0.f;
    float _frameDuration = 0.15f;
    int   _numFrames = 4;

    void update_animation(float dt);
};
