#pragma once
// Minimal base Entity using sf::Shape (not Sprite)

#include <SFML/Graphics.hpp>
#include <memory>

class Entity {
public:
    // Must be constructed with a Shape (CircleShape, RectangleShape, etc.)
    explicit Entity(std::unique_ptr<sf::Shape> shp);
    Entity() = delete;
    virtual ~Entity() = default;

    // Called once per frame – override in derived classes
    virtual void update(const float& dt);

    // Draw this entity – subclasses must implement
    virtual void render(sf::RenderWindow& window) const = 0;

    // Position helpers
    sf::Vector2f get_position() const { return _position; }
    void set_position(const sf::Vector2f& pos);
    void move(const sf::Vector2f& delta);

protected:
    std::unique_ptr<sf::Shape> _shape;  // SFML shape we draw
    sf::Vector2f _position{ 0.f, 0.f }; // cached world position
};
