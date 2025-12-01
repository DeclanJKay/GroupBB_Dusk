#pragma once

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>

enum EntType
{
    enemy,
    player,
    count //NEEDS TO BE AT THE BOTTOM, DONT REMOVE
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

class B2Helper
{
    private:
        static constexpr float b2scaleFactor = 32;
    public:
        static float SFMLtoB2(float num)
        {
            num /= b2scaleFactor;
            return num;
        }
        static b2Vec2 SFMLtoB2(sf::Vector2f nums)
        {
            nums.x /= b2scaleFactor;
            nums.y /= b2scaleFactor;
            return b2Vec2{nums.x,nums.y};
        }
        static float B2toSFML(float num)
        {
            num *= b2scaleFactor;
            return num;
        }
        static sf::Vector2f B2toSFML(b2Vec2 nums)
        {
            nums.x *= b2scaleFactor;
            nums.y *= b2scaleFactor;
            return sf::Vector2f(nums.x,nums.y);
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

class RigidEntity //DO NOT CREATE AN OBJECT OF THIS CLASS, ONLY MEANT FOR INHERITANCE
{
    private:
        sf::Vector2f GetPolySize(b2Polygon poly);
    protected:
        b2BodyId _bodyID;
    public:
        RigidEntity() = default;
        ~RigidEntity() = default;
        void RenderHitbox(sf::RenderWindow& window, sf::Color col);
        virtual void Update(const float& dt);
        virtual void Render(sf::RenderWindow& window) const = 0;
        b2BodyId getBodyID();
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
            gBox.center = b2Vec2{0,0};
            b2ShapeDef gshapedef = b2DefaultShapeDef();
            b2CreateCircleShape(_bodyID, &gshapedef, &gBox);
        }
        void Update(const float& dt) override {}
        void Render(sf::RenderWindow& window) const override {}
};