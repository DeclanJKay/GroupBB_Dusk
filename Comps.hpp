#pragma once

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <vector>

using Entity = uint32_t;

//supporting structs and enums

enum damageGroup //to specify who the bullet hits
{
    friendly,
    enemy,
    both
};

enum class EnemyTypes {
    Basic,
    Fast,
    Tank,
	shortRanged,
	Exploder,
    Medium,
	RangedMelee,
	FastExploder,
	LongRange,
	HeavyTank,
	Boss1,
	Boss2,
	Boss3,
	Boss4,
	Boss5
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
};

//can be added to entities
struct Position
{
    sf::Vector2f pos;
};

struct Velocity
{
    sf::Vector2f vel;
};

struct Health
{
    int maxHealth;
    int hp;
    damageGroup dGroup;
};

struct RenderHitboxes
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
    float friction = 20; //lower number means more floaty (20 friction seems to work well)
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
    float moveTimer = 0; //linked to moveDelay
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
    EnemyTypes type = EnemyTypes::Basic;
    bool boss = false;
};

struct WaveSpawner
{
    int lvlIndex = 0;
    const int maxLvl;
    int waveIndex = 0;
    int iniPointBudget = 0;
    int pointIncrease = 0; //point budget increase per level
    int pointBudget = 0;
    float spawnInterval = 1;
    float spawnTimer = 0;
    std::vector<sf::Vector2f> path;
};

//YOU NEED TO ADD YOUR NEW COMPONENTS HERE FOR THEM TO BE AVAILABLE ON THE ENTITIES
using AllComponents = std::tuple
<
    EnemyShootingLogic, EnemySafeMove, Friction, Position, Velocity, CircleCollider, 
    Health, RenderHitboxes, PlayerMovement, WeaponArsenal, Bullet, PlayerWeaponLogic,
    TDPathMove, EnemyType, WaveSpawner
>;