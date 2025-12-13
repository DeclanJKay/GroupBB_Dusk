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