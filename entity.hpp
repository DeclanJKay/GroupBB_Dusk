#pragma once

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>

enum EntType
{
    enemy,
    player,
    count
};

class EntityTags
{
    private:
        static EntType typeArray[(int)count];
    public:
        static void PopulateArray()
        {
            for (int i = 0; i < (int)count; i++)
            {
                typeArray[i] = (EntType)i;
            }
        }
        static EntType* GenerateTag(EntType tag)
        {
            return &typeArray[(int)tag];
        }
        static bool CheckForTag(b2BodyId bodyID, EntType tag)
        {
            EntType* iptr = (EntType*)b2Body_GetUserData(bodyID);
            if (iptr == nullptr) 
            {
                std::cout<<"This entity doesn't have a tag.\n";
                return false;
            }
            return (*iptr == tag);
        }
};

class Entity {
public:
    // Must be constructed with a Shape (CircleShape, RectangleShape, etc.)
    explicit Entity(std::unique_ptr<sf::Shape> shp);
    Entity() = delete;
    virtual ~Entity() = default;

    // Called once per frame � override in derived classes
    virtual void update(const float& dt);

    // Draw this entity � subclasses must implement
    virtual void render(sf::RenderWindow& window) const = 0;

    // Position helpers
    sf::Vector2f get_position() const { return _position; }
    void set_position(const sf::Vector2f& pos);
    void move(const sf::Vector2f& delta);

protected:
    std::unique_ptr<sf::Shape> _shape;  // SFML shape we draw
    sf::Vector2f _position{ 0.f, 0.f }; // cached world position
};

class RigidEntity
{
    private:
        sf::Vector2f GetPolySize(b2Polygon poly)
        {
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
    protected:
        b2BodyId _bodyID;
    public:
        RigidEntity() {};
        ~RigidEntity() = default;
        void RenderHitbox(sf::RenderWindow& window, sf::Color col)
        {
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

        b2BodyId getBodyID()
        {
            return _bodyID;
        }
};

//here is an example class from rigid entity
class Goon : public RigidEntity
{
    public:
        Goon() {}
        Goon(b2WorldId* worldID)
        {
            b2BodyDef def = b2DefaultBodyDef();
            def.position = (b2Vec2){12.5f, 5.00f};
            def.userData = EntityTags::GenerateTag(player);
            _bodyID = b2CreateBody(*worldID, &def);
            b2Circle gBox;
            gBox.radius = 3;
            b2ShapeDef gshapedef = b2DefaultShapeDef();
            b2CreateCircleShape(_bodyID, &gshapedef, &gBox);
        }
};