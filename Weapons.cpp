#include "Weapons.hpp"
#include "GenericHelpers.hpp"

const std::unordered_map<Weapons, int> WeaponStatsMgr::WeaponCosts = 
{
    {StartingWeapon,1}, //these are just random values 👍
    {Sniper,5},
    {Shotgun,8},
    {Shiv,1},
    {SMG,3},
    {Splurger6000,20}
};

const std::unordered_map<Weapons, std::shared_ptr<sf::Texture>>& getMap() 
{
    static const std::unordered_map<Weapons, std::shared_ptr<sf::Texture>> map = 
    {
        {StartingWeapon, FileMgr::GetTxtr("res/img/tempShopEntry.png")}, 
        {Sniper,         FileMgr::GetTxtr("res/img/tempShopEntry.png")},
        {Shotgun,        FileMgr::GetTxtr("res/img/tempShopEntry.png")},
        {Shiv,           FileMgr::GetTxtr("res/img/tempShopEntry.png")},
        {SMG,            FileMgr::GetTxtr("res/img/tempShopEntry.png")},
        {Splurger6000,   FileMgr::GetTxtr("res/img/tempShopEntry.png")}
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
            curWeapon.damage = 1;
            curWeapon.dGroup = damageGroup::enemy;
            curWeapon.fireRate = 2;
            curWeapon.pierce = 0;
            curWeapon.offset = {0, curWeapon.bulletRadius};
            curWeapon.gunTxtr = FileMgr::GetTxtr("res/img/gun.png");
            break;

        case Sniper:
            curWeapon.bulletRadius = 10;
            curWeapon.bulletSpeed = 200;
            curWeapon.bulletsShot = 1;
            curWeapon.bulletLifetime = 5;
            curWeapon.damage = 1;
            curWeapon.dGroup = damageGroup::enemy;
            curWeapon.fireRate = 2;
            curWeapon.pierce = 0;
            curWeapon.offset = {0, curWeapon.bulletRadius};
            curWeapon.gunTxtr = FileMgr::GetTxtr("res/img/gun.png");
            break;
            
        case Shotgun:
            curWeapon.bulletRadius = 10;
            curWeapon.bulletSpeed = 200;
            curWeapon.bulletsShot = 5;
            curWeapon.bulletSpread = 30;
            curWeapon.bulletLifetime = 1;
            curWeapon.damage = 1;
            curWeapon.dGroup = damageGroup::enemy;
            curWeapon.fireRate = 1;
            curWeapon.pierce = 0;
            curWeapon.offset = {0, curWeapon.bulletRadius};
            //curWeapon.gunTxtr = TxtrMgr::GetTxtr("res/img/playerAni.png");
            break;

        case Shiv:
            curWeapon.bulletRadius = 10;
            curWeapon.bulletSpeed = 200;
            curWeapon.bulletsShot = 1;
            curWeapon.bulletLifetime = 5;
            curWeapon.damage = 1;
            curWeapon.dGroup = damageGroup::enemy;
            curWeapon.fireRate = 2;
            curWeapon.pierce = 0;
            curWeapon.offset = {0, curWeapon.bulletRadius};
            curWeapon.gunTxtr = FileMgr::GetTxtr("res/img/gun.png");
            break;

        case SMG:
            curWeapon.bulletRadius = 10;
            curWeapon.bulletSpeed = 200;
            curWeapon.bulletsShot = 1;
            curWeapon.bulletLifetime = 5;
            curWeapon.damage = 1;
            curWeapon.dGroup = damageGroup::enemy;
            curWeapon.fireRate = 2;
            curWeapon.pierce = 0;
            curWeapon.offset = {0, curWeapon.bulletRadius};
            curWeapon.gunTxtr = FileMgr::GetTxtr("res/img/gun.png");
            break;

        case Splurger6000:
            curWeapon.bulletRadius = 10;
            curWeapon.bulletSpeed = 200;
            curWeapon.bulletsShot = 1;
            curWeapon.bulletLifetime = 5;
            curWeapon.damage = 1;
            curWeapon.dGroup = damageGroup::enemy;
            curWeapon.fireRate = 2;
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