#pragma once

#include "Reg.hpp"
#include "MouseHelper.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include "gameParams.hpp"
#include "EnemyStats.hpp"
#include "tile_level_loader/level_system.hpp"
#include "GenericHelpers.hpp"
#include "FileMgr.hpp"
#include "Weapons.hpp"
#include "Turrets.hpp"
#include "KeyboardHelper.hpp"

using ls = LevelSystem;

class EntityManager : public Registry
{
    public:
        void CreateButton()
        {
            
        }

        void CreateTurret(sf::Vector2i pos) //prefab for turret
        {
            auto testTur = CreateEntity();
            add<Position>(testTur, {(sf::Vector2f)pos});
            add<CircleCollider>(testTur, {20});
            add<RenderHitboxes>(testTur, {2, sf::Color::Cyan});
            add<TurretWeaponLogic>(testTur, {300});

            WeaponArsenal weaponArs;
            weaponArs.weapons.push_back(Weapon{});
            weaponArs.weapons[0].bulletLifetime = 5;
            weaponArs.weapons[0].bulletRadius = 10;
            weaponArs.weapons[0].bulletSpeed = 200;
            weaponArs.weapons[0].bulletsShot = 1;
            weaponArs.weapons[0].damage = 1;
            weaponArs.weapons[0].dGroup = damageGroup::enemy;
            weaponArs.weapons[0].fireRate = 1;
            add<WeaponArsenal>(testTur, weaponArs);
        }

        void CreateSHEnemy(Entity* player, EnemyTypes* type) //prefab for sh enemy 
        {
            auto stats = EnemyStatsManager::GetStats(*type);

            auto enemy = CreateEntity();
            add<RenderHitboxes>(enemy, RenderHitboxes{3, stats.col});
            add<Position>(enemy, Position{sf::Vector2f(300, 100)}); //door location (todo: add multiple spawnpoint?)
            add<Velocity>(enemy, Velocity{sf::Vector2f(0,0)});
            add<Friction>(enemy, Friction{20});
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
            add<RenderHitboxes>(enemy, {5, stats.col}); 
            add<EnemyType>(enemy, EnemyType{*type, false});
        }

        Entity CreatePlayer() //prefab for player
        {
            int radius = 15;
            int hp = 3;

            auto player = CreateEntity();
            add<RenderHitboxes>(player, RenderHitboxes{1, sf::Color::White});
            add<PlayerMovement>(player, PlayerMovement{50});
            add<Position>(player, Position{sf::Vector2f(300,300)});
            add<Velocity>(player, Velocity{sf::Vector2f(0,0)});
            add<Friction>(player, Friction{20});
            add<Health>(player, {hp, hp, friendly});
            add<CircleCollider>(player, CircleCollider{radius});

            WeaponArsenal playerArs;

            playerArs.weapons.push_back(WeaponStatsMgr::GetStats(Weapons::StartingWeapon));
            playerArs.weapons.push_back(WeaponStatsMgr::GetStats(Weapons::Shotgun));
            add<WeaponArsenal>(player, playerArs);
            add<PlayerWeaponLogic>(player,{4});

            //testing stuff below
            auto txtr = FileMgr::GetTxtr("res/img/playerAni.png");
            sf::Sprite sprt;
            sprt.setTexture(*txtr);
            sprt.setOrigin(sf::Vector2f{32,32});
            sprt.setTextureRect(sf::IntRect{0,0,64,64});
            add<Sprite>(player, {7, sprt, -90});

            auto gun = CreateEntity();
            add<AttachToEnt>(gun, {player, {60,0}, true});
            txtr = FileMgr::GetTxtr("res/img/gun.png");
            sprt = sf::Sprite();
            sprt.setTexture(*txtr);
            add<Sprite>(gun, Sprite{5, sprt, 90});
            add<Position>(gun, {{0,0}});
            add<WeaponKickback>(gun, {20, 0, 5});

            add<ActiveGun>(player, {gun});
            return player;
        }

        void PopulateShops(int slots)
        {
            auto shops = getAllEnt<Shop>();

            for (auto shopEnt : shops)
            {
                auto shop = get<Shop>(shopEnt);
                shop->stock.clear();
                auto costToTurs = TurretStatsManager::CostToTurrets();
                auto costToWeapons = WeaponStatsMgr::CostToWeapons();

                for (int i = 0; i < slots; i++)
                {
                    while (true)
                    {
                        std::pair<Turrets, Weapons> cur;

                       //turret
                        auto ind = GetWeightedIndex(costToTurs.size(), 1, 3);
                        auto it = GetIterator(costToTurs, ind);

                        cur.first = it->second[rand() % it->second.size()];

                        //weapon
                        ind = GetWeightedIndex(costToWeapons.size(), 1, 3);
                        auto it2 = GetIterator(costToWeapons, ind);

                        cur.second = it2->second[rand() % it2->second.size()];

                        if (!shop->stock.contains(cur))
                        {
                            shop->stock.insert(cur);
                            break;
                        }
                    }
                }
            }
        }

        void Update(const float &dt)
        {
            for (auto ent : entToBit)
            {
                auto curEnt = ent.first;
                
                if (disabled.contains(curEnt)){continue;}

                HandleVelocity(curEnt, dt);
                HandleFriction(curEnt, dt);
                HandlePlayerMovement(curEnt, dt);
                HandlePlayerWeapons(curEnt);
                ShootDelay(curEnt, dt);
                BulletLifeTime(curEnt, dt);
                HandleHealth(curEnt);
                HandleBulletColls(curEnt);
                HandleEnemySafeMove(curEnt, dt);
                HandleEnemyShooting(curEnt, dt);
                MoveAlongPath(curEnt, dt);
                SpawnEnemies(curEnt, dt);
                HandleTurretShooting(curEnt, dt);
                HandleTurretCreation(curEnt);
                HandleTurretDestruction(curEnt);
                HandleAttachedEnts(curEnt);
                HandleWeaponKickBack(curEnt, dt);
                HandleButton(curEnt);
            }
            HandleCreationAndDestruction();
        }

        void Draw(sf::RenderWindow &window)
        {
            //my god
            auto it = layermap.begin();
            while (it != layermap.end())
            {
                std::vector<int> toRemove;
                for (int i = 0; i < it->second.size(); i++)
                {
                    if (toAdd.contains(it->second[i].first)){continue;} //if not yet added dont render or remove
                    if (!Exists(it->second[i].first)){toRemove.push_back(i); continue;}
                    if (disabled.contains(it->second[i].first)){continue;} //dont render if disabled
                    switch (it->second[i].second)
                    {
                        //WHEN ADDING DRAW FUNCTIONS COPY PASTE A CASE ENTRY, REPLACE TYPE IN INDEX<> AND
                        //REMOVEIFMISSING<>() TO WHATEVER COMP YOURE TESTING FOR AND REPLACE DRAW METHOD
                        //WITH YOUR OWN ONE. MAKE SURE YOUR COMPONENT INHERITS FROM RENDERABLE  AND
                        //YOU ADDED IT TO THE SWITCH CASE IN ONADD()
                        case Index<RectShape, AllComponents>::value:
                            if (RemoveIfMissing<RectShape>(it->second[i].first, i, toRemove)){break;}
                            DrawRects(window, it->second[i].first);
                            break;

                        case Index<Sprite, AllComponents>::value:
                            if (RemoveIfMissing<Sprite>(it->second[i].first, i, toRemove)){break;}
                            DrawSprite(window, it->second[i].first);
                            break;

                        case Index<Text, AllComponents>::value:
                            if (RemoveIfMissing<Text>(it->second[i].first, i, toRemove)){break;}
                            DrawTxt(window, it->second[i].first);
                            break;

                        case Index<RenderHitboxes, AllComponents>::value:
                            if (RemoveIfMissing<RenderHitboxes>(it->second[i].first, i, toRemove)){break;}
                            DrawHitboxes(window, it->second[i].first);
                            break;
                    }
                }
                //handle removal
                auto& curVector = it->second;//shorten for simplicity
                for (int i = toRemove.size()-1; i >= 0; i--)
                {
                    //swap
                    auto temp = curVector.back();
                    curVector.back() = curVector[toRemove[i]];
                    curVector[toRemove[i]] = temp;

                    //and pop
                    curVector.pop_back();
                }
                if (curVector.size() == 0)
                {
                    it = layermap.erase(it);
                    continue;
                }
                it++;
            }
        }

    private:
        std::map<int, std::vector<std::pair<Entity, size_t>>> layermap; //variable for handling layers when rendering

        void HandleVelocity(Entity ent, const float &dt)
        {
            if (has<Position, Velocity>(ent))
            {
                get<Position>(ent)->pos += get<Velocity>(ent)->vel*dt;
            }
        }

        void HandleFriction(Entity ent, const float &dt)
        {
            if (!has<Friction, Velocity>(ent)){return;}
            auto vel = get<Velocity>(ent);
            vel->vel -= vel->vel * get<Friction>(ent)->friction * dt;
        }

        void DrawHitboxes(sf::RenderWindow &window, Entity ent)
        {
            if (has<CircleCollider, Position>(ent))
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

        void HandlePlayerMovement(Entity ent, const float& dt)
        {
            if(CheckIfPlayerRestrict()){return;}
            if (has<PlayerMovement, Velocity, Position, Friction>(ent))
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
                    auto vel = get<Velocity>(ent);
                    
                    vel->vel += dir*(float)spd*Params::SpeedMult*dt;
                }
            }
        }
    
        void SwitchGunTxtr(std::shared_ptr<sf::Texture> txtr, Entity gunEnt)
        {
            if (txtr == nullptr) {Disable(gunEnt); return;}
            Enable(gunEnt);
            if (!has<Sprite>(gunEnt)) {return;}
            auto sprite = get<Sprite>(gunEnt);
            sprite->sprt.setTexture(*txtr);
            auto size = txtr->getSize();
            sprite->sprt.setTextureRect(sf::IntRect{0,0,(int)size.x,(int)size.y});
        }

        bool HandlePlayerWeaponSwitch(WeaponArsenal* arsenal)
        {
            //numbers
            int ogInd = arsenal->selected;
            auto maxInd = (int)arsenal->weapons.size()-1;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) { arsenal->selected = 0; }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) { arsenal->selected = std::min(1, maxInd); }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) { arsenal->selected = std::min(2, maxInd); }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) { arsenal->selected = std::min(3, maxInd); }

            //scrollweheel
            arsenal->selected += MouseHelper::MouseWheelMovement();
            if (arsenal->selected < 0)
            {
                arsenal->selected = maxInd;
            }
            else if (arsenal->selected > maxInd)
            {
                arsenal->selected = 0;
            }

            return arsenal->selected != ogInd;
        }

        void HandlePlayerWeapons(Entity ent)
        {
            if(CheckIfPlayerRestrict()){return;}
            if (has<PlayerWeaponLogic, WeaponArsenal, Position>(ent))
            {
                auto arsenal = get<WeaponArsenal>(ent);
                if (HandlePlayerWeaponSwitch(arsenal))
                {
                    SwitchGunTxtr(arsenal->weapons[arsenal->selected].gunTxtr, get<ActiveGun>(ent)->gun);
                }

                auto mousePos = (sf::Vector2f)MouseHelper::GetMousePos();
                RotateSprite(ent, &mousePos);

                //shoot
                if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) { return; }
                auto weapon = &arsenal->weapons[arsenal->selected]; 
                auto pos = get<Position>(ent);
                if(Shoot(weapon, mousePos, pos->pos))
                {
                    if (!has<ActiveGun>(ent)){return;}
                    auto wkb = get<WeaponKickback>(get<ActiveGun>(ent)->gun);
                    wkb->curRecoil += wkb->recoil;
                }
            }
        }

        void RotateSprite(Entity ent, sf::Vector2f* targetPos)
        {
            if (!has<Position, Sprite>(ent)) {return;}

            auto dir = *targetPos - get<Position>(ent)->pos;
            dir /= std::sqrt(dir.x * dir.x + dir.y * dir.y);

            auto newRot = atan2(dir.y, dir.x) * 180 / M_PI;

            get<Sprite>(ent)->sprt.setRotation(newRot + get<Sprite>(ent)->rotOffset);
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
                add<RenderHitboxes>(curBullet, RenderHitboxes{8, col});
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

        void DropMoney(Health* health, Entity ent)
        {
            if (health->dGroup == damageGroup::friendly){return;}
            if (!has<EnemyType>(ent)){return;}

            auto wallets = getAllEnt<WalletPtr>();
            for (auto w : wallets)
            {
                get<WalletPtr>(w)->walletPtr->money += EnemyStatsManager::GetCost(get<EnemyType>(ent)->type);
            }
        }

        void HandleHealth(Entity ent)
        {
            if (has<Health>(ent))
            {
                auto health = get<Health>(ent);
                if (health->hp <= 0)
                {
                    //destroy ent
                    Destroy(ent);

                    //increase money in wallets
                    DropMoney(health, ent);
                    
                    //destroy gun if it has one
                    if (!has<ActiveGun>(ent)){return;}
                    Destroy(get<ActiveGun>(ent)->gun);
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
                if (bul->pierce < 0) {return;} 
                if (bul->dGroup != eHP->dGroup){continue;}
                auto dist = ePos->pos - get<Position>(curBul)->pos;
                if (std::sqrt(dist.x * dist.x + dist.y * dist.y) > (eCol->radius + get<CircleCollider>(curBul)->radius)){continue;;}
                eHP->hp -= bul->damage;
                bul->pierce--; //this is to prevent bullets hitting multiple enemies when grouped up
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
    
        void HandleEnemySafeMove(Entity ent, const float& dt)
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
            vel->vel += dir * (float)enemyMove->moveSpd * dt * Params::SpeedMult;
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
            //todo: implement some sort of logic for switching weapons?
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

            //todo: this is fairly sloppy, comeback to it if have time 
            auto pos = get<Position>(ent);
            auto pathMove = get<TDPathMove>(ent);
            if (pathMove->reachedEnd == true){Destroy(ent); return;}
            if (pathMove->target == pathMove->path.size()){pathMove->reachedEnd=true; return;}
            auto dir = pathMove->path[pathMove->target] - pos->pos;
            auto dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
            dir /= dist;
            auto change = dir*(float)pathMove->moveSpd * dt; //pathmove doesnt use friction so no need for the constant
            auto changeDist = std::sqrt(change.x * change.x + change.y * change.y);
            if (dist <= changeDist)
            {
                pathMove->target++;
            }
            pos->pos+= change;
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
                auto costEnemyPair = GetIterator(costMap, costInd);

                auto type = costEnemyPair->second[rand()%costEnemyPair->second.size()];

                auto stats = EnemyStatsManager::GetStats(type);
                CreateTDEnemy(spawner->path, &type);
                spawner->spawnTimer = spawner->spawnInterval;
                spawner->pointBudget -= costEnemyPair->first;
                if (spawner->pointBudget <= 0)
                {
                    spawner->canStart = false;
                }
                return;
            }
            
            if (spawner->lvlIndex >= spawner->maxLvl){return;} //prevent going over max lvl
            if (!spawner->canStart){return;}
            //std::cout<<"Press space to start new wave\n";
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
            if (inRange.size() == 0) {return; }
            
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

        void HandleTurretCreation(Entity ent)
        {
            if(CheckIfPlayerRestrict()){return;}
            //todo: store a variable for the grid size and replace all the hardcoded values
            if (!has<TurretHandler>(ent)){return;}
            if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {return;}
            sf::Vector2i placePos = MouseHelper::GetMousePos()/50*50 + sf::Vector2i(25,25);
            if (ls::get_tile_at((sf::Vector2f)placePos) != ls::EMPTY) {return;}
            auto turrets = getAllEnt<TurretWeaponLogic>();
            for (auto tur : turrets)
            {
                if (!has<Position>(tur)){return;}//failsafe
                if ((sf::Vector2i)get<Position>(tur)->pos == placePos)
                {
                    return;
                }
            }
            CreateTurret(placePos);
        }

        void HandleTurretDestruction(Entity ent)
        {
            if(CheckIfPlayerRestrict()){return;}
            if (!has<TurretHandler>(ent)){return;}
            if (!sf::Mouse::isButtonPressed(sf::Mouse::Right)){return;}

            sf::Vector2i selPos = MouseHelper::GetMousePos()/50*50 + sf::Vector2i(25,25);
            //todo: this wont work for any turrets that dont have shooting logic (e.g. legally distinct banana farms)
            auto allTurs = getAllEnt<TurretWeaponLogic>();
            for (auto tur : allTurs)
            {
                if (!has<Position>(tur)){continue;} //failsafe
                auto pos = (sf::Vector2i)get<Position>(tur)->pos;
                if (selPos != pos){continue;}
                Destroy(tur);
                return;
            }
        }

        void DrawSprite(sf::RenderWindow &window, Entity ent)
        {
            auto sprite = get<Sprite>(ent);
            if (has<Position>(ent))
            {
                sprite->sprt.setPosition(get<Position>(ent)->pos);
            }
            window.draw(sprite->sprt);
        }

        void HandleWeaponKickBack(Entity ent, const float& dt)
        {
            if (!has<WeaponKickback, Position, Sprite>(ent)){return;}
            auto wkb = get<WeaponKickback>(ent);
            if (wkb->curRecoil <= 0) {return;}
            auto pos = get<Position>(ent);
            auto rot = get<Sprite>(ent)->sprt.getRotation();
            rot *= M_PI/180;
            sf::Vector2f dir = {cos(rot), sin(rot)};
            pos->pos -= dir * wkb->curRecoil;
            wkb->curRecoil -= wkb->curRecoil * wkb->bounceBack * dt;
        }
        
        void HandleAttachedEnts(Entity ent)
        {
            if (!has<AttachToEnt, Position>(ent)){return;}
            auto attached = get<AttachToEnt>(ent);
            if (!has<Position>(attached->parent)){return;}
            auto pos = get<Position>(ent);
            pos->pos = get<Position>(attached->parent)->pos;

            if (attached->inheritRot && has<Sprite>(attached->parent))
            {
                auto rot = get<Sprite>(attached->parent)->sprt.getRotation();
                if (has<Sprite>(ent))
                {
                    auto sprt = get<Sprite>(ent);
                    sprt->sprt.setRotation(rot + sprt->rotOffset);
                }
                rot *= M_PI/180;
                auto dir = sf::Vector2f{cos(rot),sin(rot)};

                pos->pos += dir * attached->offset.y;
                pos->pos += sf::Vector2f(-dir.y, dir.x) * attached->offset.x;
            }
            else
            {
                pos->pos += attached->offset;
            }
        }

        void DrawTxt(sf::RenderWindow &window, Entity ent)
        {
            auto text = get<Text>(ent);
            if (has<Position>(ent)){text->txt.setPosition(get<Position>(ent)->pos);}
            window.draw(text->txt);
        }

        void DrawRects(sf::RenderWindow &window, Entity ent)
        {
            auto rect = get<RectShape>(ent);
            if (has<Position>(ent))
                rect->shape.setPosition(get<Position>(ent)->pos);
            window.draw(rect->shape);
        }

        void HandleButton(Entity ent)
        {
            //return if doesnt have button and position
            if (!has<Button, Position>(ent)){return;}

            //get all needed comps
            auto but = get<Button>(ent);
            auto mPos = MouseHelper::GetMousePos();
            auto pos = get<Position>(ent);

            //reset button variables
            but->hover = false;
            but->pressed = false;
            
            //return if not inside rectacngle hitbox
            if (mPos.x > pos->pos.x+but->size.x/2 || mPos.x < pos->pos.x-but->size.x/2) {return;}
            if (mPos.y > pos->pos.y+but->size.y/2 || mPos.y < pos->pos.y-but->size.y/2) {return;}
            but->hover = true;
            //return if button not just released
            if (!MouseHelper::ButtonReleased(sf::Mouse::Left)){return;}
            but->pressed = true;
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
    
        bool CheckIfPlayerRestrict()
        {
            auto allRestricts = getAllEnt<RestrictPlayerInput>();
            if (allRestricts.size() > 0)
            {
                return get<RestrictPlayerInput>(allRestricts[0])->restrict;
            }
            return false;
        } 
    
        void OnAdd(Entity e, int compInd, void* componentData) override
        {
            //add to layer map
            switch (compInd)
            {
                case Index<RectShape, AllComponents>::value:
                case Index<RenderHitboxes, AllComponents>::value:
                case Index<Text, AllComponents>::value:
                case Index<Sprite, AllComponents>::value:
                    //cast to renderable (so we can get the layer int)
                    Renderable* renderablePtr = static_cast<Renderable*>(componentData);
                    int layer = renderablePtr->layer;
                    layermap[layer].push_back({e,compInd});
                    break;
            }
        }
    
        template<typename comp>
        bool RemoveIfMissing(Entity ent, int i, std::vector<int>& toRemove)
        {
            if (!has<comp>(ent))
            {
                toRemove.push_back(i);
                return true;
            }
            return false;
        }
    };