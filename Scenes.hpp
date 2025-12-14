#pragma once

#include <box2d/box2d.h>
#include "Systems.hpp"
#include <unordered_map>
#include "FileMgr.hpp" // so you can use FileMgr::GetFont

#include "GenericHelpers.hpp"

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
        Entity upgradeTextEnt; 
        Entity debugTextEnt = 0;
        bool showDebug = false;
    public:
        SafeHouse(std::shared_ptr<Wallet> wallet = nullptr, std::shared_ptr<UpgradeData> upgrades = nullptr, bool playerRestrict = false);
        void Update(const float&dt, std::vector<EnemyTypes> toSpawn);
        bool NoEnemies();
        bool ApplyUpgrade(UpgradeTypes upg);
        bool SetRestrictPlayer(bool b);
        WeaponArsenal* GetPlayerArsenal();
};

class TowerDefence : public Scene
{
    Entity RestrictPlayerEnt;
    Entity turretHand;
    Entity upgradeTextEnt;
    Entity debugTextEnt = 0;
    bool showDebug = false;
    std::vector<EnemyTypes> toTransfer;
    public:
        TowerDefence(std::shared_ptr<Wallet> wallet = nullptr, std::shared_ptr<UpgradeData> upgrades = nullptr, bool playerRestrict = false);
        void Update(const float& dt, bool allEnemiesDead);
        std::vector<EnemyTypes> GetTransfers();
        std::vector<sf::Vector2f> SortPath(std::vector<sf::Vector2f> path);
        bool SetRestrictPlayer(bool b);
        TurretHandler* GetTurretHand();
};

class ShopScene : public Scene
{
    private:
        Entity buyButtons[3];
        Entity weaponButts[3];
        Entity prices[3];
        Entity restockButton;
        Entity totalMoney;
        Entity hoverDesc;

        void InitialiseShopInterface(Entity shop);
        void UpdateShopEnt(int index);
        void CreateHoverDescription();
        void ShowDesc(Turrets turret);
        void UpdateHoverDesc(std::string text);
        void ShowDesc(Weapons weapon);
        void UpdatePrices();
    public:
        ShopScene(std::shared_ptr<Wallet> wallet = nullptr);
        void Update(const float& dt, WeaponArsenal* ars, TurretHandler* turHand);
};