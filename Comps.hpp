#pragma once

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <vector>
#include <memory>
#include <set>
#include <map>

using Entity = uint32_t;

//SUPPORTING STRUCTS AND ENUMS ------------------------------------------------------------------------

enum damageGroup //to specify who the bullet hits
{
    friendly,
    enemy,
    both
};

enum EnemyTypes {
    eBasic,
    eFast,
    eTank,
	eShortRanged,
	eExploder,
    eMedium,
	eRangedMelee,
	eFastExploder,
	eLongRange,
	eHeavyTank,
	eBoss1,
	eBoss2,
	eBoss3,
	eBoss4,
	eBoss5
};

enum Turrets
{
    tBasic,
    tShotgun,
    tRapid,
    tSniper
};

enum Weapons
{
    //player weapons
    StartingWeapon,
    Sniper,
    Shotgun,
    Shiv,
    SMG,
    Splurger6000
};

//Upgrades in the game after the player beats a wave
enum UpgradeTypes
{
    uMaxHP,
    uDamage,
    uFireRate,
    uMoveSpeed,
    uBulletSpeed,
    uMoneyBonus
};

struct Weapon
{
    sf::Vector2f offset = {0,0}; //position offset based on direction (bullets already offset by collider radius by default)
    float fireRate = 0; //bullets per second
    float fireDelay = 0;
    int bulletSpeed = 0;
    int bulletSpread = 0; //degrees total, not per side (e.g. 30 will get a 30 degree cone)
    int bulletsShot = 0;
    int speedVariation = 0; //random + or - to the velocity
    float bulletLifetime = 0;
    int damage = 0;
    int bulletRadius = 0;
    int pierce = 0; //currenly doesn't do anything
    damageGroup dGroup = both; //determines who gets hurt by the bullet
    std::shared_ptr<sf::Texture> gunTxtr = nullptr;
};

struct Wallet
{
    int money = 0;
};

struct Renderable
{
    int layer;
};


//CAN BE ADDED TO ENTITIES -----------------------------------------------------------------
struct Position
{
    sf::Vector2f pos;
};

struct Velocity
{
    sf::Vector2f vel;
};

struct Acceleration
{
    sf::Vector2f targetVel;
    float maxChange;
};

struct Health
{
    int maxHealth;
    int hp;
    damageGroup dGroup;
};

struct RenderHitboxes : Renderable
{
    sf::Color col = sf::Color::White;
};

struct PlayerMovement
{
    int moveSpd = 100;
};

struct WeaponArsenal
{
    int selected = 0;
    std::vector<Weapon> weapons;
    bool switched = true;
};

struct CircleCollider
{
    int radius;
};

struct PlayerWeaponLogic
{
    int maxWapons = 4;
};

struct Bullet
{
    int damage = 1;
    int pierce = 0; //doesn't do anything currently
    damageGroup dGroup; //the damage group to hurt
    float lifeTime;
};

struct Friction
{
    float friction;
};

struct EnemySafeMove
{
    Entity target;
    int moveSpd;
    std::vector<int> range; //range per weapon
};

struct EnemyShootingLogic
{
    float moveDelay; //amount of time to stand still after a shot
    Entity target;
    sf::Vector2i switchCDrange;
    float moveTimer = 0; //linked to moveDelay
    float weaponSwitchTimer = 0;
};

struct TDPathMove
{
    bool reachedEnd = false;
    int moveSpd;
    int target = 1; //this is the index of the first path position they will head towards
    std::vector<sf::Vector2f> path;
};

struct EnemyType
{
    EnemyTypes type = EnemyTypes::eBasic;
};

struct WaveSpawner
{
    bool canStart = true;
    int lvlIndex = 0;
    int maxLvl;
    int iniPointBudget = 0;
    int pointIncrease = 0; //point budget increase per level
    int pointBudget = 0;
    float spawnInterval = 1;
    float spawnTimer = 0;
    bool waveSeenEnemy = false; // prevents upgrade offer triggering right as wave starts
    std::vector<sf::Vector2f> path;
};

struct UpgradeData
{
    // Persistent upgrade values
    int bonusMaxHP = 0;
    int bonusDamage = 0;
    float bonusFireRate = 0.f;
    int bonusMoveSpd = 0;
    int bonusBulletSpeed = 0;
    int moneyBonus = 0;

    // Pick 1 of 3 upgrade options
    bool offerActive = false;
    bool offerPrinted = false;
    int offerLevel = -1;
    int lastOfferLevel = -1;
    std::vector<UpgradeTypes> offerOptions;
};

struct TurretWeaponLogic
{
    int range;
};

struct TurretType
{
    Turrets type;
};

struct TurretHandler //for spawning turrets (SHOULD ONLY BE ONE ENTITY WITH THIS COMP PER SCENE)
{
    std::map<Turrets, int> inv;
    int selected = 0;
    float refund = 0.3f;
};

struct RestrictPlayerInput
{
    bool restrict = false;
};

struct Sprite : Renderable
{
    sf::Sprite sprt;
    float rotOffset = 0;
};

//these are from my previous attemp at this
struct AttachToEnt
{
    Entity parent;
    sf::Vector2f offset;
    bool inheritRot;
};

struct ActiveGun
{
    Entity gun;
};

struct WeaponKickback
{
    float recoil;
    float curRecoil;
    float bounceBack;
};

struct WalletPtr
{
    std::shared_ptr<Wallet> walletPtr;
};

struct Text : Renderable
{
    sf::Text txt;
};

struct Shop
{
    std::set<std::pair<Turrets, Weapons>> stock;
    std::pair<Turrets, Weapons> order[3];
    int prices[3];
};

struct RectShape : Renderable
{
    sf::RectangleShape shape;
};

struct Button
{
    sf::Vector2f size;
    bool hover = false;
    bool pressed = false;
};

struct ChangeButCol
{
    sf::Color def;
    sf::Color hover;
    sf::Color pressed;
    sf::Color cur;
};

struct UpgradeDataPtr
{
    std::shared_ptr<UpgradeData> upgradeDataPtr;
};

struct Dragable
{
    sf::Vector2f offset = {0,0};
    bool dragging = false;
};

struct Shield
{
    int amount;
};

//YOU NEED TO ADD YOUR NEW COMPONENTS HERE FOR THEM TO BE AVAILABLE ON THE ENTITIES
using AllComponents = std::tuple
<
    EnemyShootingLogic, EnemySafeMove, Friction, Position, Velocity, CircleCollider, 
    Health, RenderHitboxes, PlayerMovement, WeaponArsenal, Bullet, PlayerWeaponLogic,
    TDPathMove, EnemyType, WaveSpawner, TurretWeaponLogic, TurretHandler, RestrictPlayerInput,
    Sprite, AttachToEnt, ActiveGun, WeaponKickback, WalletPtr, Shop, Text, UpgradeDataPtr, TurretType,
    Button, RectShape, Dragable, Shield, ChangeButCol
>;