#pragma once

#include <memory>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

#include "game_systems.hpp"
#include "run_context.hpp"

class Player;

// ---------------------------------
// Maze scene
// ---------------------------------
class MazeScene : public Scene {
public:
    MazeScene() = default;

    void update(const float& dt) override;
    void render(sf::RenderWindow& window) override;
    void load() override;

    void reset();
    void set_file_path(const std::string& file_path);

private:
    std::string _file_path;
};

// ---------------------------------
// Simple ending screen
// ---------------------------------
class EndScene : public Scene {
public:
    EndScene() = default;
    void load() override;
    void update(const float& dt) override;          
    void render(sf::RenderWindow& window) override;

private:
    sf::Text _win_text;
    sf::Font _font;
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
    sf::Text _label;
    sf::Text _hpText;

    // Persistent player for this scene
    std::shared_ptr<Player> _player = nullptr;
    bool _initialised = false;

    // Attack state
    float _attackCooldown = 0.f;   // seconds until next swing
    float _attackEffectTimer = 0.f;   // time left to show visual arc
    sf::ConvexShape _attackArcShape;  // 90-degree attack wedge
    float _damageCooldown = 0.f; // time until next contact damage

    // Enemies that made it out of the tower defence
    struct Invader {
        sf::CircleShape shape;
        float speed = 60.f;           // movement speed in px/sec
    };
    std::vector<Invader> _invaders;

    void spawn_invaders(int count);
    void update_invaders(float dt);
};

// ---------------------------------
// Tower defence scene
// ---------------------------------
class TowerDefenceScene : public Scene {
public:
    TowerDefenceScene() = default;

    void load() override;
    void update(const float& dt) override;
    void render(sf::RenderWindow& window) override;

    // Run the TD simulation even when not the active scene
    void tick_simulation(float dt);

    // Safehouse pulls escaped enemies via this
    int consume_escaped_enemies();

private:
    sf::RectangleShape _background;

    sf::Font _font;
    sf::Text _label;

    std::shared_ptr<Player> _player;

    // Simple turret representation
    struct Turret {
        sf::Vector2i grid;           // tile coordinates
        sf::RectangleShape shape;    // visual
        float cooldown = 0.f;        // time until fire again
    };
    std::vector<Turret> _turrets;

    // Enemy path in world space (centres of the + tiles)
    std::vector<sf::Vector2f> _enemyPath;

    // Simple enemy representation
    struct Enemy {
        float t = 0.f;               // position along the path
        sf::CircleShape shape;       // visual
        int   hp = 3;                // basic health
        float flashTimer = 0.f;      // hit-flash timer
    };
    std::vector<Enemy> _enemies;

    // Bullets that the turrets fire
    struct Bullet {
        sf::Vector2f pos;
        sf::Vector2f vel;           // normalized direction
        float speed = 300.f;       // px/sec
        int   damage = 1;
        float ttl = 2.0f;        // seconds to live
        sf::CircleShape shape;      // visual
    };
    std::vector<Bullet> _bullets;

    float _spawnTimer = 0.f;     // time since last spawn
    bool  _initialised = false;   // only initialise TD once
    int   _escapedEnemies = 0;      // how many enemies finished the path

    void place_turret();
    void build_enemy_path();
    void spawn_enemy();
    void update_enemies(float dt);
    void update_turrets(float dt);
    void update_bullets(float dt);
};

// ---------------------------------
// Global scene handles
// ---------------------------------
struct Scenes {
    static std::shared_ptr<Scene> maze;
    static std::shared_ptr<Scene> safehouse;
    static std::shared_ptr<Scene> tower_defence;
    static std::shared_ptr<Scene> end;

    static std::shared_ptr<RunContext> runContext;
};
