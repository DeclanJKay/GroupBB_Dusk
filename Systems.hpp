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
    void CreateTurret(sf::Vector2i pos, TurretHandler* handler) // prefab for turret
    {
        auto it = GetIterator(handler->inv, handler->selected);
        it->second--;

        auto stats = TurretStatsManager::GetStats(it->first);
        auto tur = CreateEntity();
        add<Position>(tur, {(sf::Vector2f)pos});
        add<CircleCollider>(tur, {20});
        sf::Sprite sprt;
        sprt.setTexture(*stats.txtr);
        sprt.setOrigin(sprt.getLocalBounds().getSize()/2.f);
        add<Sprite>(tur, {2, sprt});
        add<TurretWeaponLogic>(tur, {stats.range});
        add<WeaponArsenal>(tur, stats.weapons);
        add<TurretType>(tur, {it->first});

        if (it->second == 0)
        {
            handler->inv.erase(it->first);
            handler->selected = std::max(handler->selected-1, 0);
        }
    }

    void CreateSHEnemy(Entity *player, EnemyTypes *type, int hp) // prefab for sh enemy
    {
        auto stats = EnemyStatsManager::GetStats(*type);

        auto enemy = CreateEntity();
        sf::Sprite sprt;
        sprt.setTexture(*stats.txtr);
        sprt.setOrigin(sprt.getLocalBounds().getSize()/2.f);
        add<Sprite>(enemy, {2, sprt});
        //add<RenderHitboxes>(enemy, RenderHitboxes{3, sf::Color::Red});
        add<Position>(enemy, Position{sf::Vector2f(300, 100)}); // door location (todo: add multiple spawnpoint?)
        add<Velocity>(enemy, Velocity{sf::Vector2f(0, 0)});
        add<Friction>(enemy, Friction{20});
        add<CircleCollider>(enemy, CircleCollider{stats.radius});
        add<Health>(enemy, {stats.hp, hp, damageGroup::enemy});
        add<EnemySafeMove>(enemy, EnemySafeMove{*player, (int)(stats.speed * 0.3f), stats.ranges});
        add<EnemyShootingLogic>(enemy, EnemyShootingLogic{stats.moveShootDelay, *player, stats.swapCDrange});
        add<EnemyType>(enemy, EnemyType{*type});

        // offset bullets based on radius
        for (int i = 0; i < stats.weapons.weapons.size(); i++)
        {
            stats.weapons.weapons[i].offset.y += stats.radius + stats.weapons.weapons[i].bulletRadius;
        }
        add<WeaponArsenal>(enemy, stats.weapons);

        // attach gun
        auto gun = CreateEntity();
        add<AttachToEnt>(gun, {enemy, {10, 0}, true});
        sprt = sf::Sprite();
        add<Sprite>(gun, Sprite{5, sprt});
        add<Position>(gun, {{}});
        add<WeaponKickback>(gun, {20, 0, 5});
        add<ActiveGun>(enemy, {gun});
    }

    void CreateTDEnemy(std::vector<sf::Vector2f> sorted, EnemyTypes *type) // prefab for td enemy
    {
        auto stats = EnemyStatsManager::GetStats(*type);

        auto enemy = CreateEntity();
        add<Position>(enemy, {sorted[0]});
        add<Health>(enemy, {stats.hp, stats.hp, damageGroup::enemy});
        add<TDPathMove>(enemy, {false, stats.speed, 1, sorted});
        add<CircleCollider>(enemy, {stats.radius});
        sf::Sprite sprt;
        sprt.setTexture(*stats.txtr);
        sprt.setOrigin(sprt.getLocalBounds().getSize()/2.f);
        add<Sprite>(enemy, {4, sprt});
        //add<RenderHitboxes>(enemy, {5, sf::Color::Red});
        add<EnemyType>(enemy, EnemyType{*type});

        if (stats.shieldAmount <= 0){return;}
        add<Shield>(enemy, {stats.shieldAmount});
    }

    Entity CreateButton(sf::Vector2f pos, sf::Vector2f size, std::string text, ChangeButCol butCols, sf::Color txtCol, int layer = 1)
    {
        auto but = CreateEntity();
        add<Position>(but, {pos});
        sf::RectangleShape shape;
        shape.setSize({size.x,size.y});
        shape.setOrigin({size.x/2, size.y/2});
        add<RectShape>(but, {layer,shape});
        add<Button>(but, {{size.x, size.y}, false, false});
        add<ChangeButCol>(but, butCols);
                
        //replace this with a sprite later
        sf::Text txt;
        txt.setFont(*FileMgr::GetFont("res/fonts/ARIAL.TTF"));
        txt.setString(text);
        txt.setColor(txtCol);
        txt.setOrigin(txt.getGlobalBounds().getSize()/2.f);
        add<Text>(but, {layer+2, txt});
        return but;
    }

    Entity CreatePlayer() // prefab for player
    {
        int radius = 15;
        int hp = 3;

        auto player = CreateEntity();
        //add<RenderHitboxes>(player, RenderHitboxes{1, sf::Color::White});
        add<PlayerMovement>(player, PlayerMovement{50});
        add<Position>(player, Position{sf::Vector2f(300, 300)});
        add<Velocity>(player, Velocity{sf::Vector2f(0, 0)});
        add<Friction>(player, Friction{20});
        add<Health>(player, {hp, hp, friendly});
        add<CircleCollider>(player, CircleCollider{radius});

        sf::Sprite sprt;
        sprt.setTexture(*FileMgr::GetTxtr("res/img/tempBody.png"));
        sprt.setOrigin(sprt.getLocalBounds().getSize()/2.f);
        add<Sprite>(player, {7, sprt});

        WeaponArsenal playerArs;

        playerArs.weapons.push_back(WeaponStatsMgr::GetStats(Weapons::StartingWeapon));
        playerArs.weapons.push_back(WeaponStatsMgr::GetStats(Weapons::Shotgun));
        add<WeaponArsenal>(player, playerArs);
        add<PlayerWeaponLogic>(player, {4});

        // attach gun
        auto gun = CreateEntity();
        add<AttachToEnt>(gun, {player, {10, 0}, true});
        sprt = sf::Sprite();
        add<Sprite>(gun, Sprite{5, sprt});
        add<Position>(gun, {{}});
        add<WeaponKickback>(gun, {20, 0, 5});
        add<ActiveGun>(player, {gun});

        return player;
    }

    void PopulateShop(Entity shopEnt)
    {
        auto costToTurs = TurretStatsManager::CostToTurrets();
        auto costToWeapons = WeaponStatsMgr::CostToWeapons();

        for (int i = 0; i < 3; i++)
        {
            RollShopItem(costToTurs, costToWeapons, shopEnt, i);
        }
    }

    bool BuyFromShop(int index, Entity shopEnt, WeaponArsenal *ars, TurretHandler *turHand)
    {
        auto shop = get<Shop>(shopEnt);
        auto wallet = get<WalletPtr>(getAllEnt<WalletPtr>()[0]);

        if (shop->prices[index] > wallet->walletPtr->money)
        {
            return false;
        }
        wallet->walletPtr->money -= shop->prices[index];

        // add to turret inv
        auto it = turHand->inv.find(shop->order[index].first);
        if (it == turHand->inv.end())
            turHand->inv.insert({shop->order[index].first, 1});
        else
            it->second++;

        // add to weapons
        if (ars->weapons.size() < 4)
        {
            ars->weapons.push_back(WeaponStatsMgr::GetStats(shop->order[index].second));
        }
        else
        {
            ars->weapons[ars->selected] = WeaponStatsMgr::GetStats(shop->order[index].second);
            ars->switched = true;
        }

        auto costToTurs = TurretStatsManager::CostToTurrets();
        auto costToWeapons = WeaponStatsMgr::CostToWeapons();
        shop->stock.erase(shop->order[index]);
        RollShopItem(costToTurs, costToWeapons, shopEnt, index);
        return true;
    }

    void Update(const float &dt)
    {
        for (auto ent : entToBit)
        {
            auto curEnt = ent.first;

            if (disabled.contains(curEnt))
            {
                continue;
            }

            HandleDragable(curEnt);
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
            HandleWeaponSwitch(curEnt);
            HandleSelTuretSwitch(curEnt);
            ToggleRenderTurInv(curEnt);
            UpdateTurInvText(curEnt);
        }
        HandleCreationAndDestruction();
    }

    void Draw(sf::RenderWindow &window)
    {
        // my god
        auto it = layermap.begin();
        while (it != layermap.end())
        {
            std::vector<int> toRemove;
            for (int i = 0; i < it->second.size(); i++)
            {
                if (toAdd.contains(it->second[i].first))
                {
                    continue;
                } // if not yet added dont render or remove
                if (!Exists(it->second[i].first))
                {
                    toRemove.push_back(i);
                    continue;
                }
                if (disabled.contains(it->second[i].first) || disabledComps.contains(it->second[i])){continue;} // dont render if disabled
                switch (it->second[i].second)
                {
                // WHEN ADDING DRAW FUNCTIONS COPY PASTE A CASE ENTRY, REPLACE TYPE IN INDEX<> AND
                // REMOVEIFMISSING<>() TO WHATEVER COMP YOURE TESTING FOR AND REPLACE DRAW METHOD
                // WITH YOUR OWN ONE. MAKE SURE YOUR COMPONENT INHERITS FROM RENDERABLE  AND
                // YOU ADDED IT TO THE SWITCH CASE IN ONADD()
                case Index<RectShape, AllComponents>::value:
                    if (RemoveIfMissing<RectShape>(it->second[i].first, i, toRemove))
                    {
                        break;
                    }
                    DrawRects(window, it->second[i].first);
                    break;

                case Index<Sprite, AllComponents>::value:
                    if (RemoveIfMissing<Sprite>(it->second[i].first, i, toRemove))
                    {
                        break;
                    }
                    DrawSprite(window, it->second[i].first);
                    break;

                case Index<Text, AllComponents>::value:
                    if (RemoveIfMissing<Text>(it->second[i].first, i, toRemove))
                    {
                        break;
                    }
                    DrawTxt(window, it->second[i].first);
                    break;

                case Index<RenderHitboxes, AllComponents>::value:
                    if (RemoveIfMissing<RenderHitboxes>(it->second[i].first, i, toRemove))
                    {
                        break;
                    }
                    DrawHitboxes(window, it->second[i].first);
                    break;
                }
            }
            // handle removal
            auto &curVector = it->second; // shorten for simplicity
            for (int i = toRemove.size() - 1; i >= 0; i--)
            {
                // swap
                auto temp = curVector.back();
                curVector.back() = curVector[toRemove[i]];
                curVector[toRemove[i]] = temp;

                // and pop
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
    std::map<int, std::vector<std::pair<Entity, size_t>>> layermap; // variable for handling layers when rendering

    void HandleVelocity(Entity ent, const float &dt)
    {
        if (has<Position, Velocity>(ent))
        {
            get<Position>(ent)->pos += get<Velocity>(ent)->vel * dt;
        }
    }

    void HandleFriction(Entity ent, const float &dt)
    {
        if (!has<Friction, Velocity>(ent))
        {
            return;
        }
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

    void HandlePlayerMovement(Entity ent, const float &dt)
    {
        if (CheckIfPlayerRestrict())
        {
            return;
        }
        if (has<PlayerMovement, Velocity, Position, Friction>(ent))
        {
            // clamp movement to screen
            ClampToScreen(ent);
            sf::Vector2f dir = {0, 0};

            // Basic WASD / Arrow movement input
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                dir.x -= 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                dir.x += 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                dir.y -= 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                dir.y += 1.f;

            if (dir.x != 0.f || dir.y != 0.f)
            {
                // Normalise direction so diagonal speed isn�t faster
                const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                dir /= len;

                // multiply by movespd
                auto spd = get<PlayerMovement>(ent)->moveSpd;
                auto vel = get<Velocity>(ent);

                vel->vel += dir * (float)spd * Params::SpeedMult * dt;
            }
        }
    }

    void SwitchGunTxtr(std::shared_ptr<sf::Texture> txtr, Entity gunEnt)
    {
        if (txtr == nullptr)
        {
            Disable(gunEnt);
            return;
        }
        Enable(gunEnt);
        if (!has<Sprite>(gunEnt))
        {
            return;
        }
        auto sprite = get<Sprite>(gunEnt);
        sprite->sprt.setTexture(*txtr);
        auto size = txtr->getSize();
        sprite->sprt.setTextureRect(sf::IntRect{0, 0, (int)size.x, (int)size.y});
    }

    void HandleSelTuretSwitch(Entity ent)
    {
        if (CheckIfPlayerRestrict()){return;}
        if (!has<TurretHandler>(ent)){return;}

        auto handler = get<TurretHandler>(ent);
        handler->selected = PlayerSwitchControls(handler->inv.size(), handler->selected);
    }

    void HandlePlayerWeaponSwitch(WeaponArsenal *arsenal)
    {
        SwitchSelWeapon(arsenal, PlayerSwitchControls(arsenal->weapons.size(), arsenal->selected));
    }

    void HandleWeaponSwitch(Entity ent)
    {
        if (!has<WeaponArsenal, ActiveGun>(ent))
        {
            return;
        }
        auto ars = get<WeaponArsenal>(ent);
        if (!ars->switched)
        {
            return;
        }
        SwitchGunTxtr(ars->weapons[ars->selected].gunTxtr, get<ActiveGun>(ent)->gun);
        ars->switched = false;
    }

    void HandlePlayerWeapons(Entity ent)
    {
        if (CheckIfPlayerRestrict()){return;}
        if (has<PlayerWeaponLogic, WeaponArsenal, Position>(ent))
        {
            auto arsenal = get<WeaponArsenal>(ent);
            HandlePlayerWeaponSwitch(arsenal);
            auto mousePos = (sf::Vector2f)MouseHelper::GetMousePos();
            RotateSprite(ent, &mousePos);

            // shoot
            if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                return;
            }
            auto weapon = &arsenal->weapons[arsenal->selected];
            auto pos = get<Position>(ent);
            Shoot(weapon, mousePos, pos->pos, ent);
        }
    }

    void RotateSprite(Entity ent, sf::Vector2f *targetPos)
    {
        if (!has<Position, Sprite>(ent))
        {
            return;
        }

        auto dir = *targetPos - get<Position>(ent)->pos;
        dir /= std::sqrt(dir.x * dir.x + dir.y * dir.y);

        auto newRot = atan2(dir.y, dir.x) * 180 / M_PI;

        get<Sprite>(ent)->sprt.setRotation(newRot + get<Sprite>(ent)->rotOffset);
    }

    bool Shoot(Weapon *weapon, sf::Vector2f target, sf::Vector2f pos, Entity ent, int range = -1) //-1 means doesn't care
    {
        if (weapon->bulletRadius <= 0)
        {
            return false;
        } // to prevent non defined weapons from shooting
        if (weapon->fireDelay > 0)
        {
            return false;
        }

        auto dir = target - (pos + weapon->offset);
        auto magnitude = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (range >= 0 && magnitude > range)
        {
            return false;
        }
        dir /= magnitude;

        sf::Vector2f bulPos = pos;
        bulPos += dir * weapon->offset.y;
        bulPos += sf::Vector2f(-dir.y, dir.x) * weapon->offset.x; // moves position along perpendicular vector
        for (int i = 0; i < weapon->bulletsShot; i++)
        {
            auto curBullet = CreateEntity();
            add<Position>(curBullet, Position{bulPos});
            add<Bullet>(curBullet, Bullet{weapon->damage, weapon->pierce, weapon->dGroup, weapon->bulletLifetime});
            auto newAngle = (std::atan2f(dir.y, dir.x) * 180 / M_PI + (rand() % (weapon->bulletSpread + 1) - weapon->bulletSpread / 2)) * M_PI / 180;
            auto newDir = sf::Vector2f(std::cosf(newAngle), std::sinf(newAngle));

            add<Velocity>(curBullet, {newDir * (float)(weapon->bulletSpeed + (rand() % (weapon->speedVariation * 2 + 1)) - weapon->speedVariation / 2)});
            add<CircleCollider>(curBullet, {weapon->bulletRadius});
            // this is added for testing purposes
            sf::Color col = sf::Color::Red;
            if (weapon->dGroup == enemy)
            {
                col = sf::Color::Green;
            }
            add<RenderHitboxes>(curBullet, RenderHitboxes{8, col});
        }
        weapon->fireDelay = 1.f / weapon->fireRate;
        if (has<ActiveGun>(ent))
        {
            if (has<WeaponKickback>(get<ActiveGun>(ent)->gun))
            {
                auto wkb = get<WeaponKickback>(get<ActiveGun>(ent)->gun);
                wkb->curRecoil += wkb->recoil;
            }
        }
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

        int bonus = 0;
        auto upgEnts = getAllEnt<UpgradeDataPtr>();
        if (upgEnts.size() > 0)
        {
            auto ptr = get<UpgradeDataPtr>(upgEnts[0])->upgradeDataPtr;
            if (ptr != nullptr) { bonus = ptr->moneyBonus; }
        }

        auto wallets = getAllEnt<WalletPtr>();
        for (auto w : wallets)
        {
            get<WalletPtr>(w)->walletPtr->money += EnemyStatsManager::GetCost(get<EnemyType>(ent)->type) + bonus;
        }
    }

    void HandleHealth(Entity ent)
    {
        if (has<Health>(ent))
        {
            auto health = get<Health>(ent);
            if (health->hp <= 0)
            {
                // destroy ent
                Destroy(ent);

                // increase money in wallets
                DropMoney(health, ent);

                // destroy gun if it has one
                if (!has<ActiveGun>(ent))
                {
                    return;
                }
                Destroy(get<ActiveGun>(ent)->gun);
            }
        }
    }

    void HandleBulletColls(Entity ent)
    {
        if (has<Bullet>(ent))
        {
            return;
        }
        if (!has<Health, CircleCollider, Position>(ent))
        {
            return;
        }
        auto bullets = getAllEnt<Bullet>();
        auto eCol = get<CircleCollider>(ent);
        auto eHP = get<Health>(ent);
        auto ePos = get<Position>(ent);
        for (auto curBul : bullets)
        {
            if (!has<CircleCollider, Position>(curBul))
            {
                continue;
            }
            auto bul = get<Bullet>(curBul);
            if (bul->pierce < 0)
            {
                return;
            }
            if (bul->dGroup != eHP->dGroup)
            {
                continue;
            }
            auto dist = ePos->pos - get<Position>(curBul)->pos;
            if (std::sqrt(dist.x * dist.x + dist.y * dist.y) > (eCol->radius + get<CircleCollider>(curBul)->radius))
            {
                continue;
                ;
            }
            if (has<Shield>(ent))
            {
                auto shield = get<Shield>(ent);
                if (shield->amount > 0)
                    shield->amount -= bul->damage;
            }
            else 
                eHP->hp -= bul->damage;
            bul->pierce--; // this is to prevent bullets hitting multiple enemies when grouped up
            Destroy(curBul);
        }
    }

    void ShootDelay(Entity ent, const float &dt)
    {
        if (has<WeaponArsenal>(ent))
        {
            auto arsenal = get<WeaponArsenal>(ent);

            for (int i = 0; i < (int)arsenal->weapons.size(); i++)
            {
                auto weapon = &arsenal->weapons[i];
                if (weapon->fireDelay <= 0)
                {
                    continue;
                }
                weapon->fireDelay = std::max(0.f, weapon->fireDelay - dt);
            }
        }
    }

    void HandleEnemySafeMove(Entity ent, const float &dt)
    {
        if (!has<EnemySafeMove, WeaponArsenal, Velocity, Position>(ent))
        {
            return;
        }
        if (has<EnemyShootingLogic>(ent))
        {
            if (get<EnemyShootingLogic>(ent)->moveTimer > 0)
            {
                return;
            }
        }
        // clamp movement to screen
        ClampToScreen(ent);
        auto enemyMove = get<EnemySafeMove>(ent);
        if (!Exists(enemyMove->target))
        {
            return;
        }

        auto vel = get<Velocity>(ent);

        sf::Vector2f dir = get<Position>(enemyMove->target)->pos - get<Position>(ent)->pos;
        float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

        if (dist <= enemyMove->range[get<WeaponArsenal>(ent)->selected])
        {
            return;
        }
        dir /= dist;
        vel->vel += dir * (float)enemyMove->moveSpd * dt * Params::SpeedMult;
    }

    void ClampToScreen(Entity ent)
    {
        if (!has<Position>(ent))
        {
            return;
        }

        float offest = 0;
        if (has<CircleCollider>(ent))
        {
            offest = get<CircleCollider>(ent)->radius;
        }

        auto pos = get<Position>(ent);
        pos->pos.x = std::clamp(pos->pos.x, offest, (float)Params::gameW - offest);
        pos->pos.y = std::clamp(pos->pos.y, offest, (float)Params::gameH - offest);
    }

    void HandleEnemyShooting(Entity ent, const float &dt)
    {
        if (!has<Position, EnemyShootingLogic, WeaponArsenal>(ent))
        {
            return;
        }
        auto shootLog = get<EnemyShootingLogic>(ent);
        if (shootLog->moveTimer > 0)
        {
            shootLog->moveTimer -= dt;
        }
        if (!Exists(shootLog->target))
        {
            return;
        }
        if (!has<Position>(shootLog->target))
        {
            return;
        }
        RotateSprite(ent, &get<Position>(shootLog->target)->pos);
        auto weaponArse = get<WeaponArsenal>(ent);
        int range = -1;
        if (has<EnemySafeMove>(ent))
            range = get<EnemySafeMove>(ent)->range[weaponArse->selected];
        if (weaponArse->weapons.size() > 1)
        {
            if (shootLog->weaponSwitchTimer <= 0)
            {
                SwitchSelWeapon(weaponArse, rand()%weaponArse->weapons.size());
                auto range = shootLog->switchCDrange.y - shootLog->switchCDrange.x;
                shootLog->weaponSwitchTimer = shootLog->switchCDrange.x + (rand()%(range+1));
            }
            else
            {
                shootLog->weaponSwitchTimer -= dt;
            }
        }
        if (Shoot(&weaponArse->weapons[weaponArse->selected], get<Position>(shootLog->target)->pos, get<Position>(ent)->pos, ent, range))
        {
            shootLog->moveTimer = shootLog->moveDelay;
        }
    }

    void MoveAlongPath(Entity ent, const float &dt)
    {
        if (!has<Position, TDPathMove>(ent))
        {
            return;
        }

        // todo: this is fairly sloppy, comeback to it if have time
        auto pos = get<Position>(ent);
        auto pathMove = get<TDPathMove>(ent);
        if (pathMove->reachedEnd == true)
        {
            Destroy(ent);
            return;
        }
        if (pathMove->target == pathMove->path.size())
        {
            pathMove->reachedEnd = true;
            return;
        }
        auto dir = pathMove->path[pathMove->target] - pos->pos;
        auto dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        dir /= dist;

        //rotate sprite towards movementd
        if (has<Sprite>(ent))
            get<Sprite>(ent)->sprt.setRotation(std::atan2f(dir.y, dir.x)*180/M_PI);

        auto change = dir * (float)pathMove->moveSpd * dt; // pathmove doesnt use friction so no need for the constant
        auto changeDist = std::sqrt(change.x * change.x + change.y * change.y);
        if (dist <= changeDist)
        {
            pathMove->target++;
        }
        pos->pos += change;
    }

    void SpawnEnemies(Entity ent, const float &dt)
    {
        if (!has<WaveSpawner>(ent)){return;}

        auto spawner = get<WaveSpawner>(ent);
        // count down timer for enemies to spawn
        if (spawner->spawnTimer > 0)
        {
            spawner->spawnTimer -= dt;
            return;
        }

        // if has budget, spawn enemy
        if (spawner->pointBudget > 0)
        {
            auto costMap = EnemyStatsManager::GetLevelCostMap(spawner->lvlIndex);

            // todo: realistically these 2 variables should be stored and only recalculated when the lvl increases
            // but it will do for now
            auto costInd = GetWeightedIndex(costMap.size(), costMap.size() / 2 + spawner->lvlIndex, 4);
            auto costEnemyPair = GetIterator(costMap, costInd);

            auto type = costEnemyPair->second[rand() % costEnemyPair->second.size()];

            spawner->pointBudget -= costEnemyPair->first;
            if (spawner->pointBudget <= 0) //if the next enemy spawn would take it under the budget, spawn boss
                type = EnemyStatsManager::GetBoss();
            spawner->spawnTimer = spawner->spawnInterval;
            CreateTDEnemy(spawner->path, &type);
            return;
        }

        if (spawner->lvlIndex >= spawner->maxLvl)
        {
            return;
        } // prevent going over max lvl
        if (!spawner->canStart)
        {
            return;
        }
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            return;
        } // start new wave when space pressed
        spawner->lvlIndex++;
        spawner->pointBudget = spawner->iniPointBudget + spawner->pointIncrease * spawner->lvlIndex;
        spawner->canStart = false;
        std::cout << "NEW WAVE\n";
    }

    void HandleTurretShooting(Entity ent, const float &dt)
    {
        if (!has<TurretWeaponLogic, WeaponArsenal, Position>(ent))
        {
            return;
        }

        // get all enemies on the path, return if none present
        auto allEnemies = getAllEnt<TDPathMove>();
        if (allEnemies.size() == 0)
        {
            return;
        }

        // get all enemies in range
        auto turPos = get<Position>(ent)->pos;
        std::vector<Entity> inRange;
        for (auto enemy : allEnemies) // todo: if we have any time left, get rid of elements from
        {                             //      allenemies vector after they get checked for memory efficiency
            if (!has<Position>(enemy))
            {
                continue;
            } // failsafe

            //if the enemy has a depleted shield, dont shoot at them
            if (has<Shield>(enemy))
            {
                if (get<Shield>(enemy)->amount <= 0)
                {
                    continue;
                }
            }
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
        RotateSprite(ent, &get<Position>(target)->pos);
        Shoot(&get<WeaponArsenal>(ent)->weapons[0], get<Position>(target)->pos, get<Position>(ent)->pos, ent);

    }

    void HandleTurretCreation(Entity ent)
    {
        if (CheckIfPlayerRestrict())
        {
            return;
        }

        // todo: store a variable for the grid size and replace all the hardcoded values
        if (!has<TurretHandler>(ent)){return;}
        auto handler = get<TurretHandler>(ent);
        if(handler->inv.size() == 0) {return;}
        if (!MouseHelper::ButtonPressed(sf::Mouse::Left)){return;}
        //return if anything is getting dragged
        for (auto drag : getAllEnt<Dragable>())
        {
            if (get<Dragable>(drag)->dragging){return;}
        }
        sf::Vector2i placePos = MouseHelper::GetMousePos() / 50 * 50 + sf::Vector2i(25, 25);
        if (ls::get_tile_at((sf::Vector2f)placePos) != ls::EMPTY)
        {
            return;
        }
        auto turrets = getAllEnt<TurretWeaponLogic>();
        //check if valid
        for (auto tur : turrets)
        {
            if (!has<Position>(tur))
            {
                return;
            } // failsafe
            if ((sf::Vector2i)get<Position>(tur)->pos == placePos)
            {
                return;
            }
        }
        CreateTurret(placePos, handler);
    }

    void HandleTurretDestruction(Entity ent)
    {
        if (CheckIfPlayerRestrict()){return;}
        if (!has<TurretHandler>(ent)){return;}
        if (!sf::Mouse::isButtonPressed(sf::Mouse::Right)){return;}

        sf::Vector2i selPos = MouseHelper::GetMousePos() / 50 * 50 + sf::Vector2i(25, 25);
        auto allTurs = getAllEnt<TurretType>();
        for (auto tur : allTurs)
        {
            if (!has<Position>(tur))
            {
                continue;
            } // failsafe
            auto pos = (sf::Vector2i)get<Position>(tur)->pos;
            if (selPos != pos)
            {
                continue;
            }
            // refund some money and destroy turret entity
            for (auto wallet : getAllEnt<WalletPtr>())
            {
                get<WalletPtr>(wallet)->walletPtr->money += TurretStatsManager::GetCost(get<TurretType>(tur)->type) * get<TurretHandler>(ent)->refund;
            }
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

    void HandleWeaponKickBack(Entity ent, const float &dt)
    {
        if (!has<WeaponKickback, Position, Sprite>(ent))
        {
            return;
        }
        auto wkb = get<WeaponKickback>(ent);
        if (wkb->curRecoil <= 0)
        {
            return;
        }
        auto pos = get<Position>(ent);
        auto rot = get<Sprite>(ent)->sprt.getRotation();
        rot *= M_PI / 180;
        sf::Vector2f dir = {cos(rot), sin(rot)};
        pos->pos -= dir * wkb->curRecoil;
        wkb->curRecoil -= wkb->curRecoil * wkb->bounceBack * dt;
    }

    void HandleAttachedEnts(Entity ent)
    {
        if (!has<AttachToEnt, Position>(ent))
        {
            return;
        }
        auto attached = get<AttachToEnt>(ent);
        if (!has<Position>(attached->parent))
        {
            return;
        }
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
            rot *= M_PI / 180;
            auto dir = sf::Vector2f{cos(rot), sin(rot)};

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
        if (has<Position>(ent))
        {
            text->txt.setPosition(get<Position>(ent)->pos);
        }
        window.draw(text->txt);
    }

    void ToggleRenderTurInv(Entity ent)
    {
        if (!has<TurretHandler, RectShape, Text>(ent, true)){return;}
        if (!KeyboardHelper::KeyPressed(sf::Keyboard::I)) {return;}

        //backdrop
        if (has<RectShape>(ent))
            Disable(ent, GetCompID<RectShape>());
        else
            Enable(ent, GetCompID<RectShape>());

        //text
        if (has<Text>(ent))
            Disable(ent, GetCompID<Text>());
        else
            Enable(ent, GetCompID<Text>());
    }

    void UpdateTurInvText(Entity ent)
    {
        if (!has<TurretHandler, RectShape, Text>(ent, true)){return;}
        auto handler = get<TurretHandler>(ent);
        auto rect = get<RectShape>(ent);
        auto text = get<Text>(ent);

        int padding = 20;
        std::string newTxt = "";

        for (int i = 0; i < handler->inv.size(); i++)
        {
            auto it = GetIterator(handler->inv, i);
            if (i == handler->selected) {newTxt+=">  ";}
            newTxt += std::to_string(i) + ": " + TurretStatsManager::GetTurretName(it->first) + " x" + std::to_string(it->second) + "\n";
        }

        text->txt.setString(newTxt);
        if (handler->inv.size() == 0) {return;}

        auto bounds = text->txt.getGlobalBounds();
        rect->shape.setSize({bounds.width + padding, bounds.height + padding});
        text->txt.setOrigin({-padding/2, -padding/2});
    }

    void HandleDragable(Entity ent)
    {
        if (CheckIfPlayerRestrict()){return;}
        if (!has<RectShape, Dragable, Position>(ent)){return;}
        auto shape = get<RectShape>(ent);
        auto drag = get<Dragable>(ent);
        auto pos = get<Position>(ent);

        auto mPos = (sf::Vector2f)MouseHelper::GetMousePos();
        if (!drag->dragging)
        {
            if (!CheckMouseInRect(shape->shape.getSize(), pos->pos,{0,0})){return;}
            else if (MouseHelper::ButtonPressed(sf::Mouse::Left)) 
            {
                drag->dragging = true;
                drag->offset = pos->pos - mPos;
            }
            else {return;}
        }
        else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {drag->dragging = false; return;}

        pos->pos = mPos + drag->offset;
    }

    void DrawRects(sf::RenderWindow &window, Entity ent)
    {
        auto rect = get<RectShape>(ent);
        if (has<Position>(ent))
            rect->shape.setPosition(get<Position>(ent)->pos);
        if (has<ChangeButCol, Button>(ent))
            rect->shape.setFillColor(get<ChangeButCol>(ent)->cur);
        window.draw(rect->shape);
    }

    void HandleButton(Entity ent)
    {
        // return if doesnt have button and position
        if (!has<Button, Position>(ent))
        {
            return;
        }

        // get all needed comps
        auto but = get<Button>(ent);
        auto pos = get<Position>(ent);
        auto origin = but->size/2.f;
        ChangeButCol* colChange = nullptr;

        // reset button variables
        but->hover = false;
        but->pressed = false;

        if (has<ChangeButCol>(ent))
        {
            colChange = get<ChangeButCol>(ent);
            colChange->cur = colChange->def;
        }

        if (!CheckMouseInRect(but->size, pos->pos, origin)){return;}

        if (colChange != nullptr)
            colChange->cur = colChange->hover;

        but->hover = true;

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if (colChange != nullptr)
                colChange->cur = colChange->pressed;
        }

        // return if button not just released
        if (!MouseHelper::ButtonReleased(sf::Mouse::Left))
        {
            return;
        }
        but->pressed = true;
    }

    void ApplyUpgrade(Entity player, UpgradeTypes upg)
        {
            if (!Exists(player)) {return;}

            // Grab shared upgrade data if it exists in this scene
            UpgradeData* upgData = nullptr;
            auto upgEnts = getAllEnt<UpgradeDataPtr>();
            if (upgEnts.size() > 0)
            {
                auto ptr = get<UpgradeDataPtr>(upgEnts[0])->upgradeDataPtr;
                if (ptr != nullptr) { upgData = ptr.get(); }
            }

            switch (upg)
            {
                case uMaxHP:
                {
                    if (upgData) { upgData->bonusMaxHP += 1; }
                    if (has<Health>(player))
                    {
                        auto hp = get<Health>(player);
                        hp->maxHealth += 1;
                        hp->hp += 1;
                    }
                } break;

                case uDamage:
                {
                    if (upgData) { upgData->bonusDamage += 1; }
                    if (has<WeaponArsenal>(player))
                    {
                        auto ars = get<WeaponArsenal>(player);
                        for (auto& w : ars->weapons) { w.damage += 1; }
                    }
                } break;

                case uFireRate:
                {
                    if (upgData) { upgData->bonusFireRate += 0.25f; }
                    if (has<WeaponArsenal>(player))
                    {
                        auto ars = get<WeaponArsenal>(player);
                        for (auto& w : ars->weapons) { w.fireRate += 0.25f; }
                    }
                } break;

                case uMoveSpeed:
                {
                    if (upgData) { upgData->bonusMoveSpd += 10; }
                    if (has<PlayerMovement>(player))
                    {
                        get<PlayerMovement>(player)->moveSpd += 10;
                    }
                } break;

                case uBulletSpeed:
                {
                    if (upgData) { upgData->bonusBulletSpeed += 50; }
                    if (has<WeaponArsenal>(player))
                    {
                        auto ars = get<WeaponArsenal>(player);
                        for (auto& w : ars->weapons) { w.bulletSpeed += 50; }
                    }
                } break;

                case uMoneyBonus:
                {
                    if (upgData) { upgData->moneyBonus += 1; }
                } break;
            }
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
        if (i > 0)
        {
            prev = weights[i - 1];
        }
        weights.push_back((float)weight + prev);
    }

        float rolled = (float)(rand()) / ((float)(RAND_MAX / weights.back()));

        // todo: this can be found faster by splitting in half
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

    void OnAdd(Entity e, int compInd, void *componentData) override
    {
        // add to layer map
        switch (compInd)
        {
        case Index<RectShape, AllComponents>::value:
        case Index<RenderHitboxes, AllComponents>::value:
        case Index<Text, AllComponents>::value:
        case Index<Sprite, AllComponents>::value:
            // cast to renderable (so we can get the layer int)
            Renderable *renderablePtr = static_cast<Renderable *>(componentData);
            int layer = renderablePtr->layer;
            layermap[layer].push_back({e, compInd});
            break;
        }
    }

    template <typename comp>
    bool RemoveIfMissing(Entity ent, int i, std::vector<int> &toRemove)
    {
        if (!has<comp>(ent))
        {
            toRemove.push_back(i);
            return true;
        }
        return false;
    }

    void SwitchSelWeapon(WeaponArsenal *ars, int newSelected)
    {
        if (ars->selected == newSelected)
        {
            return;
        }
        ars->selected = newSelected;
        ars->switched = true;
    }

    int PlayerSwitchControls(int size, int curSelected)
    {
        int newIndex = curSelected;
        auto maxInd = size - 1;

        //numbers
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)){newIndex=0;}
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)){newIndex=std::min(1, maxInd);}
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)){newIndex=std::min(2, maxInd);}
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)){newIndex=std::min(3, maxInd);}
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)){newIndex=std::min(4, maxInd);}
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6)){newIndex=std::min(5, maxInd);}
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num7)){newIndex=std::min(6, maxInd);}
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num8)){newIndex=std::min(7, maxInd);}
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num9)){newIndex=std::min(8, maxInd);}
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0)){newIndex=std::min(9, maxInd);}

        // scrollweheel
        newIndex += MouseHelper::MouseWheelMovement();
        if (newIndex < 0)
            newIndex = maxInd;
        else if (newIndex > maxInd)
            newIndex = 0;

        return newIndex;
    }

    bool CheckMouseInRect(const sf::Vector2f& size, const sf::Vector2f& pos, sf::Vector2f origin)
    {
        auto mPos = MouseHelper::GetMousePos();

        // return if not inside rectacngle hitbox
        if (mPos.x > pos.x + size.x - origin.x || mPos.x < pos.x - origin.x){return false;}
        if (mPos.y > pos.y + size.y - origin.y || mPos.y < pos.y - origin.y){return false;}
        return true;
    }

    void RollShopItem(
        std::map<int, std::vector<Turrets>> &costToTurs,
        std::map<int, std::vector<Weapons>> &costToWeapons,
        Entity shopEnt,
        int indexOfEntry)
    {
        auto shop = get<Shop>(shopEnt, true);
        while (true)
        {
            std::pair<Turrets, Weapons> cur;

            // turret
            auto ind = GetWeightedIndex(costToTurs.size(), 1, 3);
            auto it = GetIterator(costToTurs, ind);

            cur.first = it->second[rand() % it->second.size()];

            // weapon
            ind = GetWeightedIndex(costToWeapons.size(), 1, 3);
            auto it2 = GetIterator(costToWeapons, ind);

            cur.second = it2->second[rand() % it2->second.size()];

            if (!shop->stock.contains(cur))
            {
                shop->stock.insert(cur);
                shop->order[indexOfEntry] = cur;
                shop->prices[indexOfEntry] = it->first + it2->first;
                break;
            }
        }
    }
};