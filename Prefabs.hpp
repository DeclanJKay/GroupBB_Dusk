#pragma once
#include "Systems.hpp"

class Prefabs
{
    Prefabs() = delete;
    ~Prefabs() = delete;
    public:
        static void CreateSHEnemy(EntityManager* entMan);
        static void CreateTDEnemy(EntityManager* entMan);
        static Entity CreatePlayer(EntityManager* entMan);
};