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
        Entity RestrictPlayerEnt;
    public:
        SafeHouse(std::shared_ptr<Wallet> wallet = nullptr, bool playerRestrict = false);
        void Update(const float&dt, std::vector<EnemyTypes> toSpawn);
        bool NoEnemies();
        bool SetRestrictPlayer(bool b);
};

class TowerDefence : public Scene
{
    Entity RestrictPlayerEnt;
    std::vector<EnemyTypes> toTransfer;
    public:
        TowerDefence(std::shared_ptr<Wallet> wallet = nullptr, bool playerRestrict = false);
        void Update(const float& dt, bool allEnemiesDead);
        std::vector<EnemyTypes> GetTransfers();
        std::vector<sf::Vector2f> SortPath(std::vector<sf::Vector2f> path);
        bool SetRestrictPlayer(bool b);
};