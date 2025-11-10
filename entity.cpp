#include "entity.hpp"

Entity::Entity(std::unique_ptr<sf::Shape> s)
    : _shape(std::move(s)) {
}

void Entity::set_position(const sf::Vector2f& pos) {
    _position = pos;
    _shape->setPosition(_position);
}

void Entity::move(const sf::Vector2f& delta) {
    _position += delta;
    _shape->setPosition(_position);
}

void Entity::update(const float& /*dt*/) {
    // Base does nothing; override in derived classes when needed.
}
