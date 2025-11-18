#pragma once

#include "entity.hpp"
#include <SFML/Graphics.hpp>

// Simple player controlled circle
class Player : public Entity {
public:
    Player();

    // Movement update
    void update(const float& dt) override;

    // Draw the shape
    void render(sf::RenderWindow& window) const override;

    // Enable / disable tile-based collision
    // (true in Maze / Tower Defence, false in Safehouse)
    void set_use_tile_collision(bool enabled) { _use_tile_collision = enabled; }

    // --- Health API ---
    int  get_health() const { return _health; }
    int  get_max_health() const { return _maxHealth; }
    bool is_dead() const { return _health <= 0; }

    // Apply damage (clamped to >= 0)
    void take_damage(int amount);

    // Radius helper for collision with invaders
    float get_radius() const { return kRadius; }

private:
    static constexpr float kRadius = 25.f;
    static constexpr float kSpeed = 200.f; // units per second

    bool _use_tile_collision = false; // default: free movement

    // --- Health values ---
    int _maxHealth = 5;
    int _health = _maxHealth;
};
