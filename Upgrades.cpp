#include "Upgrades.hpp"
#include "Systems.hpp"        // EntityManager definition
#include "KeyboardHelper.hpp"
#include <cstdlib>            // rand
#include <iostream>
#include <sstream>


std::vector<UpgradeTypes> UpgradeManager::RollUpgradeOptions(int count)
{
    std::vector<UpgradeTypes> pool =
    {
        uMaxHP, uDamage, uFireRate, uMoveSpeed, uBulletSpeed, uMoneyBonus
    };

    std::vector<UpgradeTypes> rolled;
    while (rolled.size() < (size_t)count && rolled.size() < pool.size())
    {
        auto pick = pool[rand() % pool.size()];

        bool dupe = false;
        for (auto r : rolled)
        {
            if (r == pick) { dupe = true; break; }
        }
        if (!dupe) { rolled.push_back(pick); }
    }
    return rolled;
}

std::string UpgradeManager::UpgradeName(UpgradeTypes upg)
{
    switch (upg)
    {
        case uMaxHP:        return "+1 Max HP";
        case uDamage:       return "+1 Damage";
        case uFireRate:     return "+0.25 Fire Rate";
        case uMoveSpeed:    return "+10 Move Speed";
        case uBulletSpeed:  return "+50 Bullet Speed";
        case uMoneyBonus:   return "+1 Money per Kill";
    }
    return "Unknown Upgrade";
}

UpgradeData* UpgradeManager::GetUpgradeData(EntityManager& entMan)
{
    auto upgEnts = entMan.getAllEnt<UpgradeDataPtr>();
    if (upgEnts.size() == 0) {return nullptr;}

    auto ptr = entMan.get<UpgradeDataPtr>(upgEnts[0])->upgradeDataPtr;
    if (ptr == nullptr) {return nullptr;}

    return ptr.get();
}

void UpgradeManager::TryTriggerOffer(EntityManager& entMan, const std::vector<Entity>& spawners)
{
    auto upg = GetUpgradeData(entMan);
    if (upg == nullptr) { return; }
    if (upg->offerActive) { return; }

    for (auto spawner : spawners)
    {
        auto sp = entMan.get<WaveSpawner>(spawner);
        if (sp == nullptr) { continue; }

        // prevent triggering before the first wave has started
        if (sp->lvlIndex < 0) { continue; }

        // only trigger once the wave has finished spawning
        if (sp->pointBudget > 0) { continue; }

        // only once per level
        if (upg->lastOfferLevel == sp->lvlIndex) { continue; }

        upg->offerActive = true;
        upg->offerPrinted = false;
        upg->offerLevel = sp->lvlIndex;
        upg->offerOptions = RollUpgradeOptions();
        break;
    }
}

bool UpgradeManager::HandleOfferInput(EntityManager& entMan, Entity player)
{
    auto upg = GetUpgradeData(entMan);
    if (upg == nullptr) {return false;}
    if (!upg->offerActive) {return false;}

    if (!upg->offerPrinted)
    {
        std::cout << "\nChoose an upgrade:\n";
        for (int i = 0; i < (int)upg->offerOptions.size(); i++)
        {
            std::cout << (i + 1) << ") " << UpgradeName(upg->offerOptions[i]) << "\n";
        }
        std::cout << "\nPress 1-3 to pick.\n";
        upg->offerPrinted = true;
    }

    int choice = -1;
    if (KeyboardHelper::KeyPressed(sf::Keyboard::Num1)) { choice = 0; }
    else if (KeyboardHelper::KeyPressed(sf::Keyboard::Num2)) { choice = 1; }
    else if (KeyboardHelper::KeyPressed(sf::Keyboard::Num3)) { choice = 2; }
    else { return false; }

    if (choice < 0 || choice >= (int)upg->offerOptions.size()) {return false;}

    ApplyUpgrade(entMan, player, upg->offerOptions[choice]);


    std::cout << "Applied: " << UpgradeName(upg->offerOptions[choice]) << "\n";

    upg->lastOfferLevel = upg->offerLevel;
    upg->offerActive = false;
    upg->offerOptions.clear();

    return true;
}

bool UpgradeManager::ApplyUpgrade(EntityManager& entMan, Entity player, UpgradeTypes upg)
{
    if (!entMan.Exists(player)) {return false;}
    auto upgData = GetUpgradeData(entMan);

    switch (upg)
    {
        case uMaxHP:
            if (upgData) {upgData->bonusMaxHP += 1;}
            if (entMan.has<Health>(player))
            {
                auto hp = entMan.get<Health>(player);
                hp->maxHealth += 1;
                hp->hp += 1;
            }
            break;

        case uDamage:
            if (upgData) {upgData->bonusDamage += 1;}
            if (entMan.has<WeaponArsenal>(player))
            {
                auto ars = entMan.get<WeaponArsenal>(player);
                for (auto& w : ars->weapons) {w.damage += 1;}
            }
            break;

        case uFireRate:
            if (upgData) {upgData->bonusFireRate += 0.5f;}
            if (entMan.has<WeaponArsenal>(player))
            {
                auto ars = entMan.get<WeaponArsenal>(player);
                for (auto& w : ars->weapons) {w.fireRate += 0.5f;}
            }
            break;

        case uMoveSpeed:
            if (upgData) {upgData->bonusMoveSpd += 25;}
            if (entMan.has<PlayerMovement>(player))
            {
                entMan.get<PlayerMovement>(player)->moveSpd += 25;
            }
            break;

        case uBulletSpeed:
            if (upgData) {upgData->bonusBulletSpeed += 100;}
            if (entMan.has<WeaponArsenal>(player))
            {
                auto ars = entMan.get<WeaponArsenal>(player);
                for (auto& w : ars->weapons) {w.bulletSpeed += 100;}
            }
            break;

        case uMoneyBonus:
            if (upgData) {upgData->moneyBonus += 1;}
            break;
    }

    return true;
}

int UpgradeManager::MoneyBonus(EntityManager& entMan)
{
    auto upg = GetUpgradeData(entMan);
    if (upg == nullptr) {return 0;}
    return upg->moneyBonus;
}

std::string UpgradeManager::BuildDebugString(EntityManager& entMan, Entity player)
{
    std::ostringstream ss;

    ss << "DEBUG\n";

    // Money 
    auto wallets = entMan.getAllEnt<WalletPtr>();
    if (!wallets.empty())
    {
        auto w = entMan.get<WalletPtr>(wallets[0])->walletPtr;
        if (w) ss << "Money: " << w->money << "\n";
    }

    // Upgrade totals
    auto upg = GetUpgradeData(entMan);
    if (upg)
    {
        ss << "\nUPGRADES (run totals)\n";
        ss << "MaxHP Bonus:      " << upg->bonusMaxHP << "\n";
        ss << "Damage Bonus:     " << upg->bonusDamage << "\n";
        ss << "FireRate Bonus:   " << upg->bonusFireRate << "\n";
        ss << "MoveSpeed Bonus:  " << upg->bonusMoveSpd << "\n";
        ss << "BulletSpeed Bonus:" << upg->bonusBulletSpeed << "\n";
        ss << "Money/Kill Bonus: " << upg->moneyBonus << "\n";

        ss << "\nOfferActive: " << (upg->offerActive ? "YES" : "no") << "\n";
        ss << "OfferLevel:  " << upg->offerLevel << "\n";
        ss << "LastOffer:   " << upg->lastOfferLevel << "\n";
    }
    else
    {
        ss << "\n(No UpgradeDataPtr in this scene)\n";
    }

    // Player stats 
    if (player != 0 && entMan.Exists(player))
    {
        ss << "\nPLAYER\n";

        if (entMan.has<Health>(player))
        {
            auto hp = entMan.get<Health>(player);
            ss << "HP: " << hp->hp << "/" << hp->maxHealth << "\n";
        }

        if (entMan.has<PlayerMovement>(player))
        {
            ss << "MoveSpd: " << entMan.get<PlayerMovement>(player)->moveSpd << "\n";
        }

        if (entMan.has<WeaponArsenal>(player))
        {
            auto ars = entMan.get<WeaponArsenal>(player);
            ss << "Weapons: " << ars->weapons.size() << "  Selected: " << ars->selected << "\n";

            // Show current weapon stats (easy to spot changes)
            if (!ars->weapons.empty() && ars->selected >= 0 && ars->selected < (int)ars->weapons.size())
            {
                auto& w = ars->weapons[ars->selected];
                ss << "CurWpn Dmg: " << w.damage
                   << "  FR: " << w.fireRate
                   << "  BulletSpd: " << w.bulletSpeed << "\n";
            }
        }
    }

    ss << "\n(F3 = toggle debug)\n";
    return ss.str();
}
