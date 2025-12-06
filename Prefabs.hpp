#pragma once
#include "Systems.hpp"

class Prefabs
{
    Prefabs() = delete;
    ~Prefabs() = delete;
    public:
        static void CreateSHEnemy(EntityManager* entMan, Entity* player);
        static void CreateTDEnemy(EntityManager* entMan, std::vector<sf::Vector2f> sorted);
        static Entity CreatePlayer(EntityManager* entMan);
};