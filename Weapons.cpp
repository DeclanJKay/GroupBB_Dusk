#include "Weapons.hpp"
#include "GenericHelpers.hpp"

const std::unordered_map<Weapons, int> WeaponStatsMgr::WeaponCosts = 
{
    {StartingWeapon,4}, //these are just random values 👍
    {Sniper,8},
    {Shotgun,6},
    {Shiv,3},
    {SMG,5},
    {Splurger6000,20}
};

const std::unordered_map<Weapons, std::shared_ptr<sf::Texture>>& getMap() 
{
    static const std::unordered_map<Weapons, std::shared_ptr<sf::Texture>> map = 
    {
        {StartingWeapon, FileMgr::GetTxtr("res/img/Pistol.png")}, 
        {Sniper,         FileMgr::GetTxtr("res/img/Rifle.png")},
        {Shotgun,        FileMgr::GetTxtr("res/img/BigGun.png")},
        {Shiv,           FileMgr::GetTxtr("res/img/tempShopEntry.png")},
        {SMG,            FileMgr::GetTxtr("res/img/SMG.png")},
        {Splurger6000,   FileMgr::GetTxtr("res/img/FastGun.png")}
    };
    return map;
}

Weapon WeaponStatsMgr::GetStats(Weapons weapon)
{
    Weapon curWeapon;

    switch (weapon)
    {
        case StartingWeapon:
            curWeapon.bulletRadius = 10;
            curWeapon.bulletSpeed = 200;
            curWeapon.bulletsShot = 1;
            curWeapon.bulletLifetime = 5;
            curWeapon.damage = 2;
            curWeapon.dGroup = damageGroup::enemy;
            curWeapon.fireRate = 2;
            curWeapon.pierce = 0;
            curWeapon.offset = {0, curWeapon.bulletRadius};
            curWeapon.gunTxtr = FileMgr::GetTxtr("res/img/gun.png");
            break;

        case Sniper:
            curWeapon.bulletRadius = 10;
            curWeapon.bulletSpeed = 500;
            curWeapon.bulletsShot = 1;
            curWeapon.bulletLifetime = 5;
            curWeapon.damage = 3;
            curWeapon.dGroup = damageGroup::enemy;
            curWeapon.fireRate = 0.7f;
            curWeapon.pierce = 0;
            curWeapon.offset = {0, curWeapon.bulletRadius};
            curWeapon.gunTxtr = FileMgr::GetTxtr("res/img/gun.png");
            break;
            
        case Shotgun:
            curWeapon.bulletRadius = 10;
            curWeapon.bulletSpeed = 400;
            curWeapon.bulletsShot = 5;
            curWeapon.bulletSpread = 30;
            curWeapon.speedVariation = 30;
            curWeapon.bulletLifetime = 1;
            curWeapon.damage = 1;
            curWeapon.dGroup = damageGroup::enemy;
            curWeapon.fireRate = 0.9f;
            curWeapon.pierce = 0;
            curWeapon.offset = {0, curWeapon.bulletRadius};
            curWeapon.gunTxtr = FileMgr::GetTxtr("res/img/gun.png");
            break;

        case Shiv:
            curWeapon.bulletRadius = 50;
            curWeapon.bulletSpeed = 30;
            curWeapon.bulletsShot = 1;
            curWeapon.bulletLifetime = 0.5f;
            curWeapon.damage = 3;
            curWeapon.dGroup = damageGroup::enemy;
            curWeapon.fireRate = 5;
            curWeapon.pierce = 0;
            curWeapon.offset = {0, curWeapon.bulletRadius};
            curWeapon.gunTxtr = FileMgr::GetTxtr("res/img/gun.png");
            break;

        case SMG:
            curWeapon.bulletRadius = 10;
            curWeapon.bulletSpeed = 400;
            curWeapon.bulletsShot = 1;
            curWeapon.bulletLifetime = 3;
            curWeapon.bulletSpread = 20;
            curWeapon.speedVariation = 20;
            curWeapon.damage = 1;
            curWeapon.dGroup = damageGroup::enemy;
            curWeapon.fireRate = 3;
            curWeapon.pierce = 0;
            curWeapon.offset = {0, curWeapon.bulletRadius};
            curWeapon.gunTxtr = FileMgr::GetTxtr("res/img/gun.png");
            break;

        case Splurger6000:
            curWeapon.bulletRadius = 30;
            curWeapon.bulletSpeed = 600;
            curWeapon.bulletsShot = 2;
            curWeapon.bulletLifetime = 3;
            curWeapon.bulletSpread = 25;
            curWeapon.speedVariation = 300;
            curWeapon.damage = 2;
            curWeapon.dGroup = damageGroup::enemy;
            curWeapon.fireRate = 1.3f;
            curWeapon.pierce = 0;
            curWeapon.offset = {0, curWeapon.bulletRadius};
            curWeapon.gunTxtr = FileMgr::GetTxtr("res/img/gun.png");
            break;
    }

    return curWeapon;
};

std::map<int, std::vector<Weapons>> WeaponStatsMgr::CostToWeapons()
{
    return FlipMap(WeaponCosts);
}

std::string WeaponStatsMgr::GetWeaponName(Weapons weapon)
{
    switch (weapon)
    {
        case Weapons::Shiv :
            return "Shiv";

        case Weapons::Shotgun :
            return "Shotgun";

        case Weapons::SMG :
            return "SMG";

        case Weapons::Sniper :
            return "Sniper";

        case Weapons::Splurger6000 :
            return "Splurger6000";

        case Weapons::StartingWeapon :
            return "StartingWeapon";
    }
}

int WeaponStatsMgr::GetCost(Weapons weapon)
{
    return WeaponCosts.at(weapon);
}

std::shared_ptr<sf::Texture> WeaponStatsMgr::GetShopIcon(Weapons weapon)
{
    const auto& weaponShopIcon = getMap();
    
    auto it = weaponShopIcon.find(weapon);
    if (it == weaponShopIcon.end()) {return nullptr;}
    return it->second;
}