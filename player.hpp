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

    // This controls whether the player uses tile-based collisions (when in the TDF/maze)
    void set_use_tile_collision(bool enabled) { _use_tile_collision = enabled; }

private:
    static constexpr float kRadius = 25.f;
    static constexpr float kSpeed = 200.f; // units per second
    bool _use_tile_collision = false;       // default: free movement
};
