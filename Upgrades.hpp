#pragma once

#include "Comps.hpp"
#include <vector>
#include <string>

class EntityManager; // forward declare

class UpgradeManager
{
    UpgradeManager() = delete;
    ~UpgradeManager() = delete;

    public:
        static std::vector<UpgradeTypes> RollUpgradeOptions(int count = 3);
        static std::string UpgradeName(UpgradeTypes upg);

        static UpgradeData* GetUpgradeData(EntityManager& entMan);
        static void TryTriggerOffer(EntityManager& entMan, const std::vector<Entity>& spawners);

        static bool HandleOfferInput(EntityManager& entMan, Entity player);
        static bool ApplyUpgrade(EntityManager& entMan, Entity player, UpgradeTypes upg);

        static int MoneyBonus(EntityManager& entMan);

        static std::string BuildDebugString(EntityManager& entMan, Entity player = 0);
};
