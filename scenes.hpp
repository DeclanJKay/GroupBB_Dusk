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
#include "WaveGeneration.hpp"
#include "TDEnemy.hpp"
#include "EnemyType.hpp"

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

    // Called from TowerDefenceScene so Safehouse can keep simulating
    void tick_simulation(float dt);

    // Called when enemies escape from TD
    void spawn_invaders(const std::vector<int>& enemyTypes);

private:
    struct Invader {
        sf::CircleShape shape;
        float speed = 0.f;
        int   hp = 1;
        int   maxHp = 1;
        float flashTimer = 0.f;
        sf::Color baseColor = sf::Color::White;

        // NEW: behaviour flags copied from EnemyStats
        EnemyType type = EnemyType::Basic;
        bool  isRanged = false;
        float rangeLimit = 0.f;
        int   damage = 1;
        bool  explodes = false;
        float explosionRadius = 0.f;
        float shootCooldown = 0.f;   // ranged fire cooldown
    };

    struct EnemyBullet {
        sf::CircleShape shape;
        sf::Vector2f    vel;
        float           speed = 220.f;
        float           ttl = 3.f;
        int             damage = 1;
    };

    bool _initialised = false;

    sf::RectangleShape _background;
    sf::Font           _font;
    sf::Text           _label;
    sf::Text           _hpText;
    sf::Text           _waveText;
    sf::ConvexShape    _attackArcShape;

    std::shared_ptr<Player> _player;

    std::vector<Invader>      _invaders;
    std::vector<EnemyBullet>  _enemyBullets;

    float _attackCooldown = 0.f;
    float _attackEffectTimer = 0.f;
    float _damageCooldown = 0.f;

    void update_invaders(float dt);
    void update_enemy_bullets(float dt);
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

    // Run TD simulation (spawning, movement, turrets, bullets)
    void tick_simulation(float dt);

    // Safehouse asks which enemies escaped this tick
    std::vector<int> consume_escaped_enemies();

    // Wave UI helpers (used by SafehouseScene to show current wave)
    bool hasFinishedAllWaves()    const { return _waveManager.hasFinishedAllWaves(); }
    bool isWaitingForPlayer()     const { return _waveManager.isWaitingForPlayer(); }
    int  getCurrentLevelIndex()   const { return _waveManager.getCurrentLevelIndex(); }
    int  getCurrentWaveIndex()    const { return _waveManager.getCurrentWaveIndex(); }
    int  getWavesInCurrentLevel() const { return _waveManager.getWavesInCurrentLevel(); }

private:
    sf::RectangleShape _background;
    sf::Font           _font;
    sf::Text           _label;
    sf::Text           _waveText;

    std::shared_ptr<Player> _player;

    std::vector<TDTurret> _turrets;
    std::vector<TDEnemy>  _enemies;
    std::vector<TDBullet> _bullets;

    std::vector<sf::Vector2f> _enemyPath;
    std::vector<int>          _escapedEnemyTypes;

    bool _initialised = false;

    WaveManager _waveManager;

    void build_enemy_path();
    void update_enemies(float dt);
    void update_turrets(float dt);
    void update_bullets(float dt);
    void place_turret();
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
