#include "entity.hpp"

// Base entity constructor – takes ownership of a drawable shape
Entity::Entity(std::unique_ptr<sf::Shape> s)
    : _shape(std::move(s)) {
}

// Set world position and sync the SFML shape
void Entity::set_position(const sf::Vector2f& pos) {
    _position = pos;
    _shape->setPosition(_position);
}

// Move by a delta and update the SFML shape
void Entity::move(const sf::Vector2f& delta) {
    _position += delta;
    _shape->setPosition(_position);
}

void Entity::update(const float& /*dt*/) {
    // Base does nothing; override in derived classes when needed.
}
