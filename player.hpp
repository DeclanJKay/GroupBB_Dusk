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
