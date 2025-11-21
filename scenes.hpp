#pragma once

#include <memory>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

#include "game_systems.hpp"
#include "run_context.hpp"
#include "EnemyType.hpp"
#include "TDEnemy.hpp"
#include "td_turret.hpp"
#include "td_bullet.hpp"

class Player;

// ---------------------------------
// Simple ending / game over screen
// ---------------------------------
class EndScene : public Scene {
public:
    EndScene() = default;
    void load() override;
    void update(const float& dt) override;
    void render(sf::RenderWindow& window) override;

private:
    sf::Text _win_text;   // "Game Over" text
    sf::Font _font;       // font used for the end screen
};


// ---------------------------------
// Safehouse scene (roguelite side)
// ---------------------------------
class SafehouseScene : public Scene {
public:
    SafehouseScene() = default;

    void load() override;
    void update(const float& dt) override;
    void render(sf::RenderWindow& window) override;

    // Run safehouse logic while this scene is not active
    void tick_simulation(float dt);

private:
    sf::RectangleShape _background;

    sf::Font _font;
    sf::Text _label;   // "SAFEHOUSE" title text
    sf::Text _hpText;  // shows player HP



    // Persistent player shared across scenes
    std::shared_ptr<Player> _player = nullptr;
    bool _initialised = false;   // only create the player once

    // Melee attack state
    float _attackCooldown = 0.f;      // time until next swing
    float _attackEffectTimer = 0.f;   // time left to show visual arc
    sf::ConvexShape _attackArcShape;  // 90-degree attack wedge
    float _damageCooldown = 0.f;      // contact damage cooldown

    // Enemies that escaped the tower defence
    struct Invader {
        sf::CircleShape shape;
        float speed = 60.f;                    // movement speed in px/sec

        int   hp = 3;                          // current health
        int   maxHp = 3;                       // max health
        float flashTimer = 0.f;                // hit flash timer
        sf::Color baseColor = sf::Color(200, 50, 50); // default colour
    };
    std::vector<Invader> _invaders;

    // Spawn safehouse invaders from TD enemy type IDs
    void spawn_invaders(const std::vector<int>& enemyTypes);
    // Move invaders and handle contact damage
    void update_invaders(float dt);
};


// ---------------------------------
// Tower defence scene (grid + turrets)
// ---------------------------------
class TowerDefenceScene : public Scene {
public:
    TowerDefenceScene() = default;

    void load() override;
    void update(const float& dt) override;
    void render(sf::RenderWindow& window) override;

    // Runs even when this isn’t the active scene (background sim)
    void tick_simulation(float dt);

    // Safehouse pulls escaped enemy types from here
    std::vector<int> consume_escaped_enemies();

private:
    sf::RectangleShape _background;

    sf::Font _font;
    sf::Text _label;

    std::shared_ptr<Player> _player;

    // Turrets placed on EMPTY tiles
    std::vector<TDTurret> _turrets;

    // Enemy path in world space (centres of + tiles)
    std::vector<sf::Vector2f> _enemyPath;

    // Enemies moving along the path
    std::vector<TDEnemy> _enemies;

    // Bullets that the turrets fire
    std::vector<TDBullet> _bullets;

    float _spawnTimer = 0.f;
    bool  _initialised = false;
    std::vector<int> _escapedEnemyTypes;
    int   _totalSpawned = 0;

    void place_turret();
    void build_enemy_path();
    void spawn_enemy(EnemyType type = EnemyType::Basic);
    TDEnemy make_enemy(EnemyType type);
    void update_enemies(float dt);
    void update_turrets(float dt);
    void update_bullets(float dt);

};


// ---------------------------------
// Global scene handles (shared everywhere)
// ---------------------------------
struct Scenes {
    static std::shared_ptr<Scene> safehouse;
    static std::shared_ptr<Scene> tower_defence;
    static std::shared_ptr<Scene> end;

    // Shared run data (wave number, currency, player stats, etc.)
    static std::shared_ptr<RunContext> runContext;
};
