#pragma once

#include <box2d/box2d.h>
#include "Systems.hpp"
#include <unordered_map>

class Scene
{
    protected:
        EntityManager _entMan;
    public:
        Scene() = default;
        virtual void Update(const float& dt);
        virtual void Draw(sf::RenderWindow& window);
};

class SafeHouse : public Scene
{
    private:
        Entity player;
    public:
        SafeHouse();
        void Update(const float&dt, std::vector<EnemyTypes> toSpawn);
        bool NoEnemies();
};

class TowerDefence : public Scene
{
    std::vector<EnemyTypes> toTransfer;
    public:
        TowerDefence();
        void Update(const float& dt, bool allEnemiesDead);
        std::vector<EnemyTypes> GetTransfers();
        std::vector<sf::Vector2f> SortPath(std::vector<sf::Vector2f> path);
};