#pragma once

#include "Reg.hpp"
#include "MouseHelper.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include "gameParams.hpp"
#include "EnemyStats.hpp"

class EntityManager : public Registry
{
    public:
        void CreateSHEnemy(Entity* player, EnemyTypes* type) //prefab for sh enemy 
        {
            auto stats = EnemyStatsManager::GetStats(*type);

            auto enemy = CreateEntity();
            add<RenderHitboxes>(enemy, RenderHitboxes{stats.col});
            add<Position>(enemy, Position{sf::Vector2f(300, 100)});
            add<Velocity>(enemy, Velocity{sf::Vector2f(0,0)});
            add<Friction>(enemy, Friction{(float)stats.friction});
            add<CircleCollider>(enemy, CircleCollider{stats.radius});
            add<Health>(enemy, {stats.hp, stats.hp, damageGroup::enemy});
            add<EnemySafeMove>(enemy, EnemySafeMove{*player, (int)(stats.speed*0.3f), stats.ranges});
            add<EnemyShootingLogic>(enemy, EnemyShootingLogic{stats.moveShootDelay, *player});
            add<EnemyType>(enemy, EnemyType{*type, false});

            //offset bullets based on radius
            for (int i = 0; i < stats.weapons.weapons.size(); i++)
            {
                stats.weapons.weapons[i].offset.y += stats.radius + stats.weapons.weapons[i].bulletRadius;
            }
            add<WeaponArsenal>(enemy, stats.weapons);
        }

        void CreateTDEnemy(std::vector<sf::Vector2f> sorted, EnemyTypes* type) //prefab for td enemy
        {
            auto stats = EnemyStatsManager::GetStats(*type);

            auto enemy = CreateEntity();
            add<Position>(enemy, {sorted[0]});
            add<Health>(enemy, {stats.hp, stats.hp, damageGroup::enemy});
            add<TDPathMove>(enemy, {false, stats.speed, 1, sorted});
            add<CircleCollider>(enemy, {stats.radius});
            add<RenderHitboxes>(enemy, {stats.col}); 
            add<EnemyType>(enemy, EnemyType{*type, false});
        }

        Entity CreatePlayer() //prefab for player
        {
            int radius = 15;
            int hp = 3;

            auto player = CreateEntity();
            add<RenderHitboxes>(player, RenderHitboxes{sf::Color::White});
            add<PlayerMovement>(player, PlayerMovement{100});
            add<Position>(player, Position{sf::Vector2f(300,300)});
            add<Velocity>(player, Velocity{sf::Vector2f(0,0)});
            add<Friction>(player, Friction{20});
            add<Health>(player, {hp, hp, friendly});
            add<CircleCollider>(player, CircleCollider{radius});

            WeaponArsenal playerArs;

            playerArs.weapons.push_back(Weapon{});
            playerArs.weapons[0].bulletRadius = 10;
            playerArs.weapons[0].bulletSpeed = 200;
            playerArs.weapons[0].bulletsShot = 1;
            playerArs.weapons[0].bulletLifetime = 5;
            playerArs.weapons[0].damage = 1;
            playerArs.weapons[0].dGroup = damageGroup::enemy;
            playerArs.weapons[0].fireRate = 2;
            playerArs.weapons[0].pierce = 0;
            playerArs.weapons[0].offset = {0, (float)(radius + playerArs.weapons[0].bulletRadius)};
            add<WeaponArsenal>(player, playerArs);
            add<PlayerWeaponLogic>(player,{4});

            return player;
        }

        void Update(const float &dt)
        {
            for (auto ent : entToBit)
            {
                auto curEnt = ent.first;
                
                HandleVelocity(curEnt, dt);
                HandleFriction(curEnt, dt);
                HandlePlayerMovement(curEnt);
                HandlePlayerWeapons(curEnt);
                ShootDelay(curEnt, dt);
                BulletLifeTime(curEnt, dt);
                HandleHealth(curEnt);
                HandleBulletColls(curEnt);
                HandleEnemySafeMove(curEnt);
                HandleEnemyShooting(curEnt, dt);
                MoveAlongPath(curEnt, dt);
                SpawnEnemies(curEnt, dt);
                HandleTurretShooting(curEnt, dt);
            }
            HandleCreationAndDestruction();
        }

        void Draw(sf::RenderWindow &window)
        {
            for (auto ent : entToBit)
            {
                auto curEnt = ent.first;
                DrawHitboxes(window, curEnt);
            }
        }

    private:
        void HandleVelocity(Entity ent, const float &dt)
        {
            if (has<Position, Velocity>(ent))
            {
                get<Position>(ent)->pos += get<Velocity>(ent)->vel*dt;
            }
        }

        void HandleFriction(Entity ent, const float &dt)
        {
            if (has<Velocity, Friction>(ent))
            {
                auto vel = get<Velocity>(ent);
                vel->vel -= (get<Friction>(ent)->friction*vel->vel)*dt;
            }
        }

        void DrawHitboxes(sf::RenderWindow &window, Entity ent)
        {
            if (has<CircleCollider, RenderHitboxes, Position>(ent))
            {
                auto collider = get<CircleCollider>(ent);
                auto pos = get<Position>(ent);
                auto render = get<RenderHitboxes>(ent);

                sf::CircleShape cir;
                cir.setRadius(collider->radius);
                cir.setPosition(pos->pos);
                cir.setOrigin(sf::Vector2f(cir.getRadius(), cir.getRadius()));
                cir.setFillColor(render->col);

                auto curPos = cir.getPosition();
                window.draw(cir);
            }
        }

        void HandlePlayerMovement(Entity ent)
        {
            if (has<PlayerMovement, Velocity, Position>(ent))
            {
                //clamp movement to screen
                ClampToScreen(ent);
                sf::Vector2f dir = {0,0};

                // Basic WASD / Arrow movement input
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  dir.x -= 1.f;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) dir.x += 1.f;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    dir.y -= 1.f;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  dir.y += 1.f;

                if (dir.x != 0.f || dir.y != 0.f) 
                {
                    // Normalise direction so diagonal speed isn�t faster
                    const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                    dir /= len;

                    //multiply by movespd
                    auto spd = get<PlayerMovement>(ent)->moveSpd;
                    auto vel = &get<Velocity>(ent)->vel;
                    *vel += dir*(float)spd;
                }
            }
        }
    
        void HandlePlayerWeapons(Entity ent)
        {
            if (has<PlayerWeaponLogic, WeaponArsenal, Position>(ent))
            {
                auto arsenal = get<WeaponArsenal>(ent);
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) { arsenal->selected = 0; }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) { arsenal->selected  = 1; }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) { arsenal->selected  = 2; }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) { arsenal->selected  = 3; }

                arsenal->selected = std::min(arsenal->selected, (int)arsenal->weapons.size()-1);

                //shootgun
                if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) { return; }
                auto weapon = &arsenal->weapons[arsenal->selected]; 
                auto mousePos = MouseHelper::GetMousePos();
                auto pos = get<Position>(ent);
                Shoot(weapon, (sf::Vector2f)mousePos, pos->pos);
            }
        }

        bool Shoot(Weapon* weapon, sf::Vector2f target, sf::Vector2f spawnPos, int range = -1)//-1 means doesn't care
        {
            if (weapon->bulletRadius <= 0) { return false; } //to prevent non defined weapons from shooting
            if (weapon->fireDelay > 0) { return false; }

            auto dir = target - spawnPos;
            auto magnitude = std::sqrt(dir.x * dir.x + dir.y * dir.y);
            if (range >= 0 && magnitude > range) {return false;}
            dir /= magnitude;

            sf::Vector2f bulPos = spawnPos;
            bulPos += dir * weapon->offset.y;
            bulPos += sf::Vector2f(-dir.y, dir.x) * weapon->offset.x; //moves position along perpendicular vector
            for (int i = 0; i < weapon->bulletsShot; i++)
            {
                auto curBullet = CreateEntity();
                add<Position>(curBullet, Position{bulPos});
                add<Bullet>(curBullet, Bullet{weapon->damage, weapon->pierce, weapon->dGroup, weapon->bulletLifetime});
                auto newAngle = (std::atan2f(dir.y, dir.x)*180/M_PI + (rand() % (weapon->bulletSpread+1) - weapon->bulletSpread/2))*M_PI/180;
                auto newDir = sf::Vector2f(std::cosf(newAngle), std::sinf(newAngle));

                add<Velocity>(curBullet, {newDir * (float)(weapon->bulletSpeed+(rand() % (weapon->speedVariation*2+1))-weapon->speedVariation/2)});
                add<CircleCollider>(curBullet, {weapon->bulletRadius});
                //this is added for testing purposes
                sf::Color col = sf::Color::Red;
                if (weapon->dGroup == enemy) { col = sf::Color::Green; }
                add<RenderHitboxes>(curBullet, RenderHitboxes{col});
            }
            weapon->fireDelay = 1.f/weapon->fireRate;
            return true;
        }
    
        void BulletLifeTime(Entity ent, const float &dt)
        {
            if (has<Bullet>(ent))
            {
                auto bul = get<Bullet>(ent);
                bul->lifeTime -= dt;

                if (bul->lifeTime <= 0)
                {
                    Destroy(ent);
                }
            }
        }

        void HandleHealth(Entity ent)
        {
            if (has<Health>(ent))
            {
                auto health = get<Health>(ent);
                if (health->hp <= 0)
                {
                    Destroy(ent);
                }
            }
        }

        void HandleBulletColls(Entity ent)
        {
            if (has<Bullet>(ent)){return;}
            if (!has<Health, CircleCollider, Position>(ent)){return;}
            auto bullets = getAllEnt<Bullet>();
            auto eCol = get<CircleCollider>(ent);
            auto eHP = get<Health>(ent);
            auto ePos = get<Position>(ent);
            for (auto curBul : bullets)
            {
                if (!has<CircleCollider, Position>(curBul)){continue;}
                auto bul = get<Bullet>(curBul);
                if (bul->dGroup != eHP->dGroup){continue;}
                auto dist = ePos->pos - get<Position>(curBul)->pos;
                if (std::sqrt(dist.x * dist.x + dist.y * dist.y) > (eCol->radius + get<CircleCollider>(curBul)->radius)){continue;;}
                eHP->hp -= bul->damage;
                Destroy(curBul);
                
            }
        }

        void ShootDelay(Entity ent, const float &dt)
        {
            if(has<WeaponArsenal>(ent))
            {
                auto arsenal = get<WeaponArsenal>(ent);

                for (int i = 0; i < (int)arsenal->weapons.size(); i++)
                {
                    auto weapon = &arsenal->weapons[i];
                    if (weapon->fireDelay <= 0) {continue;}
                    weapon->fireDelay = std::max(0.f, weapon->fireDelay - dt);
                }
            }
        }
    
        void HandleEnemySafeMove(Entity ent)
        {
            if (!has<EnemySafeMove, WeaponArsenal, Velocity, Position>(ent)){return;}
            if (has<EnemyShootingLogic>(ent))
            {
                if(get<EnemyShootingLogic>(ent)->moveTimer > 0) {return;}
            }
            //clamp movement to screen
            ClampToScreen(ent);
            auto enemyMove = get<EnemySafeMove>(ent);
            if (!Exists(enemyMove->target)){return;}

            auto vel = get<Velocity>(ent);

            sf::Vector2f dir = get<Position>(enemyMove->target)->pos - get<Position>(ent)->pos;
            float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

            if (dist <= enemyMove->range[get<WeaponArsenal>(ent)->selected]) {return;}
            dir /= dist;
            vel->vel += dir * (float)enemyMove->moveSpd;
        }

        void ClampToScreen(Entity ent)
        {
            if (!has<Position>(ent)){return;}

            float offest = 0;
            if (has<CircleCollider>(ent))
            {
                offest = get<CircleCollider>(ent)->radius;
            }

            auto pos = get<Position>(ent);
            pos->pos.x = std::clamp(pos->pos.x, offest, (float)Params::gameW-offest);
            pos->pos.y = std::clamp(pos->pos.y, offest, (float)Params::gameH-offest);
        }
    
        void HandleEnemyShooting(Entity ent, const float& dt)
        {
            if (!has<Position, EnemyShootingLogic, WeaponArsenal>(ent)){return;}
            auto shootLog = get<EnemyShootingLogic>(ent);
            if (shootLog->moveTimer > 0) {shootLog->moveTimer -= dt;}
            if (!Exists(shootLog->target)) {return;}
            if (!has<Position>(shootLog->target)){return;}
            auto weaponArse = get<WeaponArsenal>(ent);
            int range = -1;
            if (has<EnemySafeMove>(ent))
            {
                auto sigma = get<EnemySafeMove>(ent)->range[weaponArse->selected];
                range = get<EnemySafeMove>(ent)->range[weaponArse->selected];
            }
            if (Shoot(&weaponArse->weapons[weaponArse->selected], get<Position>(shootLog->target)->pos, get<Position>(ent)->pos, range))
            {
                if (shootLog->moveDelay <= 0){return;}
                shootLog->moveTimer = shootLog->moveDelay;
            }
            
        }
    
        void MoveAlongPath(Entity ent, const float& dt)
        {
            if (!has<Position, TDPathMove>(ent)){return;}
            auto pos = get<Position>(ent);
            auto pathMove = get<TDPathMove>(ent);
            if (pathMove->reachedEnd == true){Destroy(ent); return;}
            if (pathMove->target == pathMove->path.size()){pathMove->reachedEnd=true; return;}
            auto dir = pathMove->path[pathMove->target] - pos->pos;
            auto dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
            if (dist <= pathMove->moveSpd/100.f)
            {
                pos->pos = pathMove->path[pathMove->target];
                pathMove->target++;
                return;
            }
            dir /= dist;
            pos->pos += dir*(float)pathMove->moveSpd/100.f;
        }
    
        void SpawnEnemies(Entity ent, const float& dt)
        {
            if (!has<WaveSpawner>(ent)){return;}

            auto spawner = get<WaveSpawner>(ent);
            
            //count down timer for enemies to spawn
            if (spawner->spawnTimer > 0) { spawner->spawnTimer -= dt; return; }

            //if has budget, spawn enemy
            if (spawner->pointBudget > 0)
            {
                auto costMap = EnemyStatsManager::GetLevelCostMap(spawner->lvlIndex);

                //todo: realistically these 2 variables should be stored and only recalculated when the lvl increases
                //but it will do for now
                auto costInd = GetWeightedIndex(costMap.size(), costMap.size()/2+spawner->lvlIndex, 4);
                auto cost = EnemyStatsManager::GetSortedKeys(&costMap)[costInd];
                auto enemiesAtCost = costMap.at(cost);

                auto type = enemiesAtCost[rand()%enemiesAtCost.size()];

                auto stats = EnemyStatsManager::GetStats(type);
                CreateTDEnemy(spawner->path, &type);
                spawner->spawnTimer = spawner->spawnInterval;
                spawner->pointBudget -= cost;
                if (spawner->pointBudget <= 0)
                {
                    spawner->canStart = false;
                }
                return;
            }
            
            if (spawner->lvlIndex >= spawner->maxLvl){return;} //prevent going over max lvl
            if (!spawner->canStart){return;}
            std::cout<<"Press space to start new wave\n";
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){return;} //start new wave when space pressed
            spawner->lvlIndex++;
            spawner->pointBudget = spawner->iniPointBudget + spawner->pointIncrease*spawner->lvlIndex;
            std::cout<<"NEW WAVE\n";
        }

        void HandleTurretShooting(Entity ent, const float& dt)
        {
            if (!has<TurretWeaponLogic, WeaponArsenal, Position>(ent)){return;}

            //get all enemies on the path, return if none present
            auto allEnemies = getAllEnt<TDPathMove>();
            if (allEnemies.size() == 0) {return;}

            //get all enemies in range
            auto turPos = get<Position>(ent)->pos;
            std::vector<Entity> inRange;
            for (auto enemy : allEnemies)   //todo: if we have any time left, get rid of elements from
            {                               //      allenemies vector after they get checked for memory efficiency
                if (!has<Position>(enemy)){continue;} //failsafe
                auto pos = get<Position>(enemy)->pos;

                auto dist = pos - turPos;
                auto magnitude = std::sqrt(dist.x * dist.x + dist.y * dist.y);

                if (magnitude > get<TurretWeaponLogic>(ent)->range) {continue;}
                inRange.push_back(enemy);
            }

            Entity target = inRange.front();
            int maxProgress = get<TDPathMove>(target)->target;
            for (int i = 1; i < inRange.size(); i++)
            {
                auto curProgress = get<TDPathMove>(inRange[i])->target;
                if (curProgress <= maxProgress) {continue;}
                maxProgress = curProgress;
                target = inRange[i];
            }
            
            Shoot(&get<WeaponArsenal>(ent)->weapons[0], get<Position>(target)->pos, get<Position>(ent)->pos);
        }

        //helper for spawner logic
         int GetWeightedIndex(int size, float focalPoint, float spread)
         {
            std::vector<float> weights;
            for (int i = 0; i < size; i++)
            {
                //gaussian function
                float weight = (float)std::exp(-std::pow(i - focalPoint, 2) / (2 * std::pow(spread, 2)));

                float prev = 0;
                if (i > 0) {prev = weights[i-1];}
                weights.push_back((float)weight + prev);
            }

            float rolled = (float)(rand()) / ((float)(RAND_MAX/weights.back()));

            //todo: this can be found faster by splitting in half
            for (int i = 0; i < weights.size(); i++)
            {
                if (rolled < weights[i])
                {
                    return i;
                }
            }

            return -1;
         }
    };