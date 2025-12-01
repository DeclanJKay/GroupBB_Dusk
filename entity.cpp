#include "entity.hpp"

// Base entity constructor � takes ownership of a drawable shape
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

EntType EntityTags::typeArray[(int)count];

b2BodyId RigidEntity::getBodyID()
{
    return _bodyID;
}

sf::Vector2f RigidEntity::GetPolySize(b2Polygon poly)
{
    //returns the size of a polygon (in b2 scale)
    sf::Vector2f xRange = sf::Vector2f(poly.vertices[0].x, poly.vertices[0].x);
    sf::Vector2f yRange = sf::Vector2f(poly.vertices[0].y, poly.vertices[0].y);
    for (int i = 1; i < sizeof(poly.vertices)/sizeof(b2Vec2); i++)
    {
        if (poly.vertices[i].x < xRange.x) {xRange.x = poly.vertices[i].x;}
        else if (poly.vertices[i].x > xRange.y) {xRange.y = poly.vertices[i].x;}

        if (poly.vertices[i].y < yRange.x) {yRange.x = poly.vertices[i].y;}
        else if (poly.vertices[i].y > yRange.y) {yRange.y = poly.vertices[i].y;}
    }
    return sf::Vector2f(xRange.y-xRange.x, yRange.y-yRange.x);
}

//base do nothing
void RigidEntity::Update(const float& dt){}
void RigidEntity::Render(sf::RenderWindow& window) const {}

void RigidEntity::RenderHitbox(sf::RenderWindow& window, sf::Color col)
{
    //can only render a circle or a square (ADD ROTATION BTW YOU MORON)
    int shapeCount = b2Body_GetShapeCount(_bodyID);
    b2ShapeId shapeIDs[shapeCount];
    b2Body_GetShapes(_bodyID, shapeIDs, shapeCount);

    for (int i = 0; i < shapeCount; i++)
    {
        auto type = b2Shape_GetType(shapeIDs[0]);

        if (type == b2_polygonShape)
        {
            auto rectB2 = b2Shape_GetPolygon(shapeIDs[i]);
            sf::RectangleShape rectSF;
            auto posB2 = b2Body_GetTransform(_bodyID);
            rectSF.setPosition(posB2.p.x*32, posB2.p.y*32);
            auto size = GetPolySize(rectB2);
            rectSF.setSize(sf::Vector2f(size.x*32, size.y*32));
            rectSF.setOrigin(size.x*32/2, size.y*32/2);
            rectSF.setFillColor(col);

            window.draw(rectSF);
            std::cout<<"size: " << rectSF.getPosition().x << " " << rectSF.getPosition().y << "\n";
        }
        else if (type == b2_circleShape)
        {
            auto cirB2 = b2Shape_GetCircle(shapeIDs[i]);
            sf::CircleShape cirSF;
            auto posB2 = b2Body_GetTransform(_bodyID);
            cirSF.setPosition(posB2.p.x*32, posB2.p.y*32);
            cirSF.setRadius(cirB2.radius*32);
            cirSF.setFillColor(col);
            cirSF.setOrigin(cirB2.radius*32,cirB2.radius*32);
            window.draw(cirSF);
        }
    }
}