#pragma once

#include "entity.hpp"

// Simple player controlled circle
class Player : public Entity {
public:
    Player();

    // Movement update
    void update(const float& dt) override;

    // Draw the shape
    void render(sf::RenderWindow& window) const override;

private:
    static constexpr float kRadius = 25.f;
    static constexpr float kSpeed = 200.f; // units per second
};
