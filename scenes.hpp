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

class Player;

// ---------------------------------
// Maze scene (old maze lab reused)
// ---------------------------------
class MazeScene : public Scene {
public:
    MazeScene() = default;

    void update(const float& dt) override;
    void render(sf::RenderWindow& window) override;
    void load() override;

    // Reload the current maze file and reset the player position
    void reset();
    // Set which maze text file this scene should use
    void set_file_path(const std::string& file_path);

private:
    std::string _file_path;   // path to the maze level file
};

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
    sf::Text _label;  // "TOWER DEFENCE"

    // Player can walk around the TD grid to place turrets
    std::shared_ptr<Player> _player;

    std::vector<TDTurret> _turrets;

    // Enemy path in world space (centres of the + tiles)
    std::vector<sf::Vector2f> _enemyPath;

    // Simple enemy representation 
    std::vector<TDEnemy> _enemies;


    // Bullets fired by turrets
    struct Bullet {
        sf::Vector2f pos;
        sf::Vector2f vel;      // normalised direction
        float speed = 300.f;   // px/sec
        int   damage = 1;
        float ttl = 2.0f;      // time to live
        sf::CircleShape shape; // visual
    };
    std::vector<Bullet> _bullets;

    float _spawnTimer = 0.f;                // time since last enemy spawn
    bool  _initialised = false;             // only set up TD once
    std::vector<int> _escapedEnemyTypes;    // types of enemies that finished the path
    int   _totalSpawned = 0;                // total enemies spawned so far

    // Helpers for TD logic
    void place_turret();
    void build_enemy_path();
    void spawn_enemy(EnemyType type = EnemyType::Basic);
    void update_enemies(float dt);
    void update_turrets(float dt);
    void update_bullets(float dt);
};


// ---------------------------------
// Global scene handles (shared everywhere)
// ---------------------------------
struct Scenes {
    static std::shared_ptr<Scene> maze;
    static std::shared_ptr<Scene> safehouse;
    static std::shared_ptr<Scene> tower_defence;
    static std::shared_ptr<Scene> end;

    // Shared run data (wave number, currency, player stats, etc.)
    static std::shared_ptr<RunContext> runContext;
};
