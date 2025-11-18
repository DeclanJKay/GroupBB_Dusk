#include "scenes.hpp"
#include "player.hpp"
#include "tile_level_loader/level_system.hpp"
#include "game_parameters.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <unordered_map>
#include <iostream>
#include <cmath>
#include <algorithm>

using ls = LevelSystem;
using param = Parameters;

// ------------------------------------------------------------
// Global scene handles (defined here, declared in scenes.hpp)
// ------------------------------------------------------------
std::shared_ptr<Scene>      Scenes::maze = nullptr;
std::shared_ptr<Scene>      Scenes::safehouse = nullptr;
std::shared_ptr<Scene>      Scenes::tower_defence = nullptr;
std::shared_ptr<Scene>      Scenes::end = nullptr;
std::shared_ptr<RunContext> Scenes::runContext = nullptr;

// ---------------------------------------------------------------------
// Key edge-trigger helper
//  - Returns true only on the exact frame the key goes from up -> down
//  - Used for actions like placing turrets and swapping scenes
// ---------------------------------------------------------------------
bool keyPressedOnce(sf::Keyboard::Key key) {
    static std::unordered_map<sf::Keyboard::Key, bool> keyStates;

    bool isPressed = sf::Keyboard::isKeyPressed(key);
    bool wasPressed = keyStates[key];

    keyStates[key] = isPressed;

    return (isPressed && !wasPressed);
}

// ============================================================================
// MazeScene
// ============================================================================
void MazeScene::set_file_path(const std::string& file_path) {
    _file_path = file_path;
}

void MazeScene::load() {
    // Colours
    ls::set_color(ls::EMPTY, sf::Color(30, 30, 30));
    ls::set_color(ls::WALL, sf::Color(180, 180, 180));
    ls::set_color(ls::START, sf::Color(80, 255, 80));
    ls::set_color(ls::END, sf::Color(255, 80, 80));

    // Player entity
    auto player = std::make_shared<Player>();
    player->set_use_tile_collision(true); // Maze uses walls / ends
    _entities.push_back(player);

    reset();
}

void MazeScene::reset() {
    ls::load_level(_file_path, 50.f);
    if (!_entities.empty()) {
        _entities.front()->set_position(ls::get_start_position());
    }
}

void MazeScene::update(const float& dt) {
    Scene::update(dt);

    if (_entities.empty()) return;
    const sf::Vector2f p = _entities.front()->get_position();

    try {
        if (ls::get_tile_at(p) == ls::END) {
            if (_file_path == std::string(param::maze_1)) {
                _file_path = param::maze_2;
                reset();
                return;
            }

            unload();
            GameSystem::set_active_scene(Scenes::end);
            return;
        }
    }
    catch (...) {
        // ignore out of bounds
    }
}

void MazeScene::render(sf::RenderWindow& window) {
    ls::render(window);
    Scene::render(window);
}

// ============================================================================
// SafehouseScene (roguelite side)
// ============================================================================
void SafehouseScene::load() {
    _background.setSize({
        static_cast<float>(param::game_width),
        static_cast<float>(param::game_height)
        });
    _background.setFillColor(sf::Color(30, 15, 15));

    // UI font 
    if (!_font.loadFromFile("res/fonts/ARIAL.TTF")) {
        std::cerr << "Failed to load font: res/fonts/ARIAL.TTF\n";
    }

    _label.setFont(_font);
    _label.setString("SAFEHOUSE");
    _label.setCharacterSize(32);
    _label.setFillColor(sf::Color::White);
    _label.setPosition(20.f, 20.f);

	//Player HP text
    _hpText.setFont(_font);
    _hpText.setCharacterSize(24);
    _hpText.setFillColor(sf::Color::White);
    _hpText.setPosition(20.f, 60.f);   // a bit below the title
    _hpText.setString("HP: 0/0");

	// Make the attack arc shape
    _attackArcShape.setPointCount(3);
    _attackArcShape.setFillColor(sf::Color(255, 255, 255, 60));

    // First time only: create the player
    if (!_initialised) {
        _player = std::make_shared<Player>();
        _player->set_use_tile_collision(false); // Safehouse ignores tiles

        _player->set_position({
            param::game_width * 0.5f,
            param::game_height * 0.5f
            });

        _initialised = true;
    }

    // Hook existing player into entity list
    _entities.clear();
    if (_player) {
        _entities.push_back(_player);
    }
}

// Spawn invaders in the safehouse based on escaped TD enemy types
void SafehouseScene::spawn_invaders(const std::vector<int>& enemyTypes) {
    for (int typeId : enemyTypes) {
        Invader inv;

        // Defaults
        float radius = 20.f;
        inv.speed = 60.f;
        int   hp = 3;
        sf::Color color(200, 50, 50); // basic red

        // Map from TD enemy type to safehouse stats
        TowerDefenceScene::EnemyType type =
            static_cast<TowerDefenceScene::EnemyType>(typeId);

        switch (type) {
        case TowerDefenceScene::EnemyType::Basic:
            radius   = 20.f;
            inv.speed = 60.f;
            hp       = 3;                         // same as TD
            color    = sf::Color(200, 50, 50);    // red-ish
            break;

        case TowerDefenceScene::EnemyType::Fast:
            radius   = 16.f;
            inv.speed = 110.f;
            hp       = 2;                         // same as TD
            color    = sf::Color(255, 200, 0);    // yellow/orange
            break;

        case TowerDefenceScene::EnemyType::Tank:
            radius   = 24.f;
            inv.speed = 40.f;
            hp       = 6;                         // same as TD
            color    = sf::Color(150, 0, 200);    // purple-ish
            break;
        }

        inv.hp       = hp;
        inv.maxHp    = hp;
        inv.baseColor = color;

        inv.shape.setRadius(radius);
        inv.shape.setOrigin(radius, radius);
        inv.shape.setFillColor(color);

        // Stack them roughly in a column on the right
        float x = static_cast<float>(param::game_width) - 100.f;
        float y = 150.f + static_cast<float>(_invaders.size()) * 50.f;
        if (y > param::game_height - 100.f) {
            y = 150.f;
        }

        inv.shape.setPosition(x, y);

        _invaders.push_back(inv);
    }
}

// Move invaders towards the player, handle contact damage and hit flash
void SafehouseScene::update_invaders(float dt) {
    if (_invaders.empty() || !_player) return;

    // Player data
    sf::Vector2f playerPos = _player->get_position();
    float        playerR = _player->get_radius();

    for (auto& inv : _invaders) {
        sf::Vector2f pos = inv.shape.getPosition();
        sf::Vector2f dir = playerPos - pos;

        float lenSq = dir.x * dir.x + dir.y * dir.y;
        if (lenSq > 1.0f) { // avoid division by zero when very close
            float len = std::sqrt(lenSq);
            sf::Vector2f norm = dir / len;

            pos += norm * inv.speed * dt;
            inv.shape.setPosition(pos);
        }

        // --- Contact damage to player ---
        sf::Vector2f diff = playerPos - pos;
        float distSq = diff.x * diff.x + diff.y * diff.y;
        float combinedR = playerR + inv.shape.getRadius();

        if (distSq <= combinedR * combinedR && _damageCooldown <= 0.f) {
            _player->take_damage(1);    // 1 damage per contact to player
            _damageCooldown = 1.0f;     // 1 second of invulnerability
        }

        // --- Hit flash for invader (if recently damaged) ---
        if (inv.flashTimer > 0.f) {
            inv.flashTimer -= dt;
            float t = std::max(inv.flashTimer / 0.15f, 0.f);

            // Lerp towards white then back to baseColor
            sf::Color c;
            c.r = static_cast<sf::Uint8>(
                inv.baseColor.r + (255 - inv.baseColor.r) * t
                );
            c.g = static_cast<sf::Uint8>(
                inv.baseColor.g + (255 - inv.baseColor.g) * t
                );
            c.b = static_cast<sf::Uint8>(
                inv.baseColor.b + (255 - inv.baseColor.b) * t
                );
            c.a = 255;
            inv.shape.setFillColor(c);
        }
        else {
            inv.shape.setFillColor(inv.baseColor);
        }
    }

}



void SafehouseScene::update(const float& dt) {
    // Safehouse player entity 
    Scene::update(dt);

    // TD simulation continues in the background
    if (Scenes::tower_defence) {
        // We know this really is a TowerDefenceScene
        auto td = std::static_pointer_cast<TowerDefenceScene>(Scenes::tower_defence);
        td->tick_simulation(dt);

        // Pull escaped enemy types and spawn them as invaders here
        auto escapedTypes = td->consume_escaped_enemies();
        if (!escapedTypes.empty()) {
            spawn_invaders(escapedTypes);
        }
    }

    // --- Update HP text from player health ---
    if (_player) {
        int hp = _player->get_health();
        int maxHp = _player->get_max_health();

        _hpText.setString(
            "HP: " + std::to_string(hp) + "/" + std::to_string(maxHp)
        );
    }

    // --- Attack + damage timers ---
    if (_attackCooldown > 0.f) {
        _attackCooldown -= dt;
    }
    if (_attackEffectTimer > 0.f) {
        _attackEffectTimer -= dt;
    }
    if (_damageCooldown > 0.f) {
        _damageCooldown -= dt;
    }
    // Melee attack in a 90-degree arc towards the mouse
    bool doAttack = false;
    if (_attackCooldown <= 0.f && keyPressedOnce(sf::Keyboard::Space)) {
        doAttack = true;
        _attackCooldown = 0.5f;
    }

    if (doAttack && _player) {
        const float attackRadius = 80.f;
        const float attackRadiusSq = attackRadius * attackRadius;
        const float cosHalfAngle = 0.70710678f; // cos(45°) = 90° cone

        sf::Vector2f center = _player->get_position();

        // Get mouse position *relative to the game window* and convert
        // to world coordinates (so it matches the player's coord space)
        sf::RenderWindow& window = GameSystem::get_window();
        sf::Vector2i mousePix = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePix);

        // Direction from player to mouse in the same coord space
        sf::Vector2f toMouse = mousePos - center;
        float        lenSqMouse = toMouse.x * toMouse.x + toMouse.y * toMouse.y;
        sf::Vector2f forward(1.f, 0.f);

        if (lenSqMouse > 1.f) {
            float lenMouse = std::sqrt(lenSqMouse);
            forward = toMouse / lenMouse;
        }

        // Apply 1 damage to any invader inside the cone
        std::vector<Invader> survivors;
        survivors.reserve(_invaders.size());

        for (auto& inv : _invaders) {
            sf::Vector2f d = inv.shape.getPosition() - center;
            float        lenSq = d.x * d.x + d.y * d.y;

            bool hit = false;

            if (lenSq < 1.f) {
                // Very close = auto-hit
                hit = true;
            }
            else if (lenSq <= attackRadiusSq) {
                float len = std::sqrt(lenSq);
                sf::Vector2f nd = d / len;

                float dot = nd.x * forward.x + nd.y * forward.y;

                if (dot >= cosHalfAngle) {
                    // Inside 90° arc
                    hit = true;
                }
            }

            if (hit) {
                // Basic attack does 1 damage
                inv.hp -= 1;
                inv.flashTimer = 0.15f; // brief flash
            }

            if (inv.hp > 0) {
                survivors.push_back(inv);
            }
            // If hp <= 0, we simply don't push it -> enemy dies
        }

        _invaders.swap(survivors);

        // Visual arc setup
        const float cosA = 0.70710678f;
        const float sinA = 0.70710678f;

        sf::Vector2f left(
            forward.x * cosA - forward.y * sinA,
            forward.x * sinA + forward.y * cosA
        );
        sf::Vector2f right(
            forward.x * cosA + forward.y * sinA,
            -forward.x * sinA + forward.y * cosA
        );

        _attackArcShape.setPoint(0, center);
        _attackArcShape.setPoint(1, center + left * attackRadius);
        _attackArcShape.setPoint(2, center + right * attackRadius);

        _attackEffectTimer = 0.12f;
    }


    // --- Update invaders chasing the player ---
    update_invaders(dt);

    // --- Death check ---
    if (_player && _player->is_dead()) {
        // Clear any remaining invaders (optional, just to be tidy)
        _invaders.clear();

        // Make sure the EndScene exists
        if (!Scenes::end) {
            Scenes::end = std::make_shared<EndScene>();
        }

        // Switch to Game Over screen
        GameSystem::set_active_scene(Scenes::end);
        return;
    }

    // Swap between Safehouse and Tower Defence using Shift
    if (keyPressedOnce(sf::Keyboard::LShift) || keyPressedOnce(sf::Keyboard::RShift)) {
        GameSystem::set_active_scene(Scenes::tower_defence);
        return;
    }
}


void SafehouseScene::render(sf::RenderWindow& window) {
    window.draw(_background);
    Scene::render(window); // player

    for (const auto& inv : _invaders) {
        window.draw(inv.shape);
    }

    if (_attackEffectTimer > 0.f) {
        window.draw(_attackArcShape);
    }

    window.draw(_label);
    window.draw(_hpText);   // show HP of player 
}

// ============================================================================
// TowerDefenceScene
// ============================================================================
void TowerDefenceScene::load() {
    _background.setSize({
        static_cast<float>(param::game_width),
        static_cast<float>(param::game_height)
        });
    _background.setFillColor(sf::Color(5, 5, 20));

    if (!_font.loadFromFile("res/fonts/ARIAL.TTF")) {
        std::cerr << "Failed to load font: res/fonts/ARIAL.TTF\n";
    }

    _label.setFont(_font);
    _label.setString("TOWER DEFENCE");
    _label.setCharacterSize(32);
    _label.setFillColor(sf::Color::White);
    _label.setPosition(20.f, 20.f);

    const float tileSize = 50.f;

    // Configure level tile colours for TD
    if (!_initialised) {
        ls::set_color(ls::EMPTY, sf::Color(10, 10, 30));
        ls::set_color(ls::WALL, sf::Color(60, 60, 80));
        ls::set_color(ls::WAYPOINT, sf::Color(120, 120, 120));
        ls::set_color(ls::START, sf::Color(80, 255, 80));
        ls::set_color(ls::END, sf::Color(255, 80, 80));

		// Load the TD level file
        ls::load_level(param::td_1, tileSize);

        _turrets.clear();
        _enemies.clear();
        _bullets.clear();
        _enemyPath.clear();
        _spawnTimer = 0.f;
        _escapedEnemyTypes.clear();

        // Build the path (+ tiles) enemies will follow
        build_enemy_path();

        // Create the shared player for TD mode
        _entities.clear();
        _player = std::make_shared<Player>();
        _player->set_use_tile_collision(true);
        _player->set_position({ 150.f, 100.f });
        _entities.push_back(_player);

        _initialised = true;
        std::cout << "[TD] Initialised once.\n";
    }
    else {
        // When re-entering TD, keep existing state and just re-hook the player
        _entities.clear();
        if (_player) {
            _entities.push_back(_player);
        }
        std::cout << "[TD] Resumed with existing state.\n";
    }
}

// Runs the TD simulation even if this scene is not currently active
void TowerDefenceScene::tick_simulation(float dt) {
    if (!_enemyPath.empty()) {
        _spawnTimer += dt;
        const float spawnInterval = 2.0f;
        while (_spawnTimer >= spawnInterval) {
            _spawnTimer -= spawnInterval;

            // Very simple spawn progression / wave logic:
            // - Start with Basic
            // - After 5 spawns, mix in Fast
            // - After 15 spawns, mix in Tank
            EnemyType type = EnemyType::Basic;

            if (_totalSpawned >= 15) {
                // After 15 spawns: occasionally tanks + fasts
                if (_totalSpawned % 5 == 0) {
                    type = EnemyType::Tank;
                }
                else {
                    type = EnemyType::Fast;
                }
            }
            else if (_totalSpawned >= 5) {
                // After 5 spawns: mix basic + fast
                if (_totalSpawned % 3 == 0) {
                    type = EnemyType::Fast;
                }
                else {
                    type = EnemyType::Basic;
                }
            }
            else {
                // First few: only basic enemies
                type = EnemyType::Basic;
            }

            spawn_enemy(type);
        }
    }

    update_enemies(dt);
    update_turrets(dt);
    update_bullets(dt);
}

// Build list of world-space positions enemies move through (from + tiles)
void TowerDefenceScene::build_enemy_path() {
    _enemyPath.clear();

    const int w = ls::get_width();
    const int h = ls::get_height();
    const float tileSize = 50.f;

    std::vector<sf::Vector2i> waypoints;
    waypoints.reserve(w * h);

    // Collect all WAYPOINT tiles from the level
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            sf::Vector2i grid(x, y);
            if (ls::get_tile(grid) == ls::WAYPOINT) {
                waypoints.push_back(grid);
            }
        }
    }

    if (waypoints.empty()) {
        std::cerr << "No WAYPOINT tiles found for enemy path.\n";
        return;
    }

    // Pick the "start" waypoint: smallest x, then smallest y
    sf::Vector2i start = waypoints[0];
    for (const auto& p : waypoints) {
        if (p.x < start.x || (p.x == start.x && p.y < start.y)) {
            start = p;
        }
    }

    auto index = [w](sf::Vector2i p) {
        return p.y * w + p.x;
        };

    std::vector<bool> visited(static_cast<size_t>(w * h), false);
    std::vector<sf::Vector2i> ordered;
    ordered.reserve(waypoints.size());

    sf::Vector2i current = start;
    ordered.push_back(current);
    visited[static_cast<size_t>(index(current))] = true;

    // 4-connected neighbor directions
    const sf::Vector2i dirs[4] = {
        { 1,  0 },
        { -1, 0 },
        { 0,  1 },
        { 0, -1 }
    };

    // Walk the connected chain of WAYPOINT tiles
    bool extended = true;
    while (extended) {
        extended = false;

        for (const auto& d : dirs) {
            sf::Vector2i next = current + d;
            if (next.x < 0 || next.y < 0 || next.x >= w || next.y >= h) {
                continue;
            }

            if (ls::get_tile(next) == ls::WAYPOINT &&
                !visited[static_cast<size_t>(index(next))]) {

                ordered.push_back(next);
                visited[static_cast<size_t>(index(next))] = true;
                current = next;
                extended = true;
                break;
            }
        }
    }

    // Convert grid coords to world positions (center of each tile)
    _enemyPath.reserve(ordered.size());
    for (const auto& grid : ordered) {
        sf::Vector2f tilePos = ls::get_tile_position(grid);
        _enemyPath.push_back(tilePos + sf::Vector2f(tileSize * 0.5f, tileSize * 0.5f));
    }

    std::cout << "Enemy path built with " << _enemyPath.size() << " nodes.\n";
}

TowerDefenceScene::Enemy TowerDefenceScene::make_enemy(EnemyType type) {
    Enemy e;
    e.type = type;
    e.t = 0.f;
    e.flashTimer = 0.f;

    // Defaults
    float radius = 15.f;
    int   hp = 3;
    float speed = 60.f;
    sf::Color color = sf::Color::Red;

    switch (type) {
    case EnemyType::Basic:
        // Default basic enemy
        radius = 15.f;
        hp = 3;
        speed = 60.f;
        color = sf::Color::Red;
        break;

    case EnemyType::Fast:
        // Weaker but very quick
        radius = 12.f;
        hp = 2;
        speed = 110.f;
        color = sf::Color(255, 200, 0); // yellow/orange
        break;

    case EnemyType::Tank:
        // Slow but chunky
        radius = 18.f;
        hp = 6;
        speed = 40.f;
        color = sf::Color(150, 0, 200); // purple-ish
        break;
    }

    e.shape.setRadius(radius);
    e.shape.setOrigin(radius, radius);
    e.baseColor = color;
    e.shape.setFillColor(color);
    e.shape.setPosition(_enemyPath.front());

    e.hp = hp;
    e.maxHp = hp;
    e.speed = speed;

    return e;
}


void TowerDefenceScene::spawn_enemy(EnemyType type) {
    if (_enemyPath.size() < 2) return;

    Enemy e = make_enemy(type);
    e.shape.setPosition(_enemyPath.front());

    _enemies.push_back(e);
    _totalSpawned++;
}


// Move enemies along the path and handle escape + hit flashing
void TowerDefenceScene::update_enemies(float dt) {
    // Need at least 2 points to form a path
    if (_enemyPath.size() < 2) return;

    const float tileSize = 50.f;

    // ---- Move each enemy along the path ----
    for (auto& e : _enemies) {
        // 't' is parametric distance along the path measured in "tile lengths".
        // Example: t = 0.0 means at node 0
        //          t = 1.0 means at node 1
        //          t = 2.5 means halfway between node 2 and 3
        //
        // We convert speed (pixels/sec) into "tile-lengths per second":
        //   distancePixels / tileSize  -> distance in tiles
        float deltaT = (e.speed * dt) / tileSize;
        e.t += deltaT;

        // The integer part of t tells us which segment we’re on (0 → 1, 1 → 2, etc.)
        int   segment = static_cast<int>(e.t);
        // The fractional part tells us how far along that segment we are [0..1)
        float local = e.t - segment;

        // If we’ve gone past the last segment, clamp enemy to the final node
        if (segment >= static_cast<int>(_enemyPath.size()) - 1) {
            e.shape.setPosition(_enemyPath.back());
        }
        else {
            // Otherwise, interpolate between path[segment] and path[segment + 1]
            const sf::Vector2f& a = _enemyPath[static_cast<size_t>(segment)];
            const sf::Vector2f& b = _enemyPath[static_cast<size_t>(segment + 1)];
            e.shape.setPosition(a + (b - a) * local);
        }

        // ---- Hit flash colour ----
        // When e.flashTimer > 0 we temporarily tint the enemy to show it was hit
        if (e.flashTimer > 0.f) {
            e.flashTimer -= dt;
            float t = std::max(e.flashTimer / 0.2f, 0.f); // 0.2s total flash

            // We blend between a bright hit colour and its base colour.
            // Here we just drive green/blue up slightly while flashing.
            sf::Color c;
            c.r = static_cast<sf::Uint8>(255);
            c.g = static_cast<sf::Uint8>(180 + 75 * t);
            c.b = static_cast<sf::Uint8>(180 + 75 * t);
            e.shape.setFillColor(c);
        }
        else {
            // Reset back to normal colour once flashTimer hits zero
            e.shape.setFillColor(e.baseColor);
        }
    }

    // ---- Handle enemies reaching the end of the path ----
    // Enemies that get to the last node are considered "escaped" and will respawn
    // in the safehouse as invaders. Others remain in the _enemies list.
    std::vector<Enemy> alive;
    alive.reserve(_enemies.size());

    for (const auto& e : _enemies) {
        int segment = static_cast<int>(e.t);

        // If the enemy is still on a valid segment, keep it alive
        if (segment < static_cast<int>(_enemyPath.size()) - 1) {
            alive.push_back(e);
        }
        else {
            // Past the last segment -> it escaped.
            // We record its type so the Safehouse scene can spawn
            // the same kind of invader.
            _escapedEnemyTypes.push_back(static_cast<int>(e.type));
        }
    }

    // Replace the current enemy list with only the ones still on the path
    _enemies.swap(alive);
}

// Place a turret on the tile the player is standing on (if valid)
void TowerDefenceScene::place_turret() {
    if (!_player) return;

    const float tileSize = 50.f;

    // Convert player's world position into grid coordinates (tile indices)
    sf::Vector2f pos = _player->get_position();
    sf::Vector2i grid(
        static_cast<int>(pos.x / tileSize),
        static_cast<int>(pos.y / tileSize)
    );

    LevelSystem::Tile tile;
    try {
        tile = ls::get_tile(grid);
    }
    catch (...) {
        // If grid is outside the level, just ignore the request
        return;
    }

    // Only allow placing on EMPTY tiles (can't place on walls, path, etc.)
    if (tile != ls::EMPTY) {
        return;
    }

    // Prevent placing multiple turrets on the same tile
    for (const auto& t : _turrets) {
        if (t.grid == grid) {
            return;
        }
    }

    // Create a new turret snapped to the tile position
    Turret turret;
    turret.grid = grid;
    turret.cooldown = 0.f;  // can fire immediately
    turret.shape.setSize({ tileSize, tileSize });
    turret.shape.setPosition(ls::get_tile_position(grid));
    turret.shape.setFillColor(sf::Color(0, 200, 255)); // cyan-ish

    _turrets.push_back(turret);
}

// Handle turret targeting and firing bullets at enemies
void TowerDefenceScene::update_turrets(float dt) {
    // If there are no enemies or no turrets, we don’t need to do anything
    if (_enemies.empty() || _turrets.empty()) return;

    const float tileSize = 50.f;
    const float rangePixels = tileSize * 3;                 // range = 3 tiles
    const float rangeSq = rangePixels * rangePixels;    // squared range
    const float fireInterval = 0.5f;                         // 0.5s between shots

    for (auto& t : _turrets) {
        // Handle individual turret cooldown
        if (t.cooldown > 0.f) {
            t.cooldown -= dt;
            continue;
        }

        // Turret position in the middle of its tile
        sf::Vector2f turretPos =
            t.shape.getPosition() + sf::Vector2f(tileSize * 0.5f, tileSize * 0.5f);

        // Find the closest enemy in range
        Enemy* bestEnemy = nullptr;
        float  bestDistSq = rangeSq; // we compare squared distances for speed

        for (auto& e : _enemies) {
            if (e.hp <= 0) continue; // skip dead enemies (will be cleaned later)

            sf::Vector2f diff = e.shape.getPosition() - turretPos;
            float d2 = diff.x * diff.x + diff.y * diff.y;

            // New best candidate if it's closer than anything we've seen so far
            if (d2 < bestDistSq) {
                bestDistSq = d2;
                bestEnemy = &e;
            }
        }

        if (bestEnemy) {
            // ---- Fire a bullet at the chosen enemy ----
            Bullet b;
            b.pos = turretPos;

            // Direction from turret to enemy
            sf::Vector2f toEnemy = bestEnemy->shape.getPosition() - turretPos;
            float len = std::sqrt(toEnemy.x * toEnemy.x + toEnemy.y * toEnemy.y);

            // Normalise direction; if the length is 0 just give it a zero vector
            b.vel = (len > 0.f) ? (toEnemy / len) : sf::Vector2f(0.f, 0.f);
            b.speed = 300.f;   // pixels per second
            b.damage = 1;      // standard bullet damage
            b.ttl = 2.0f;   // bullet disappears after 2 seconds regardless

            // Visual setup for the bullet (small white circle)
            b.shape.setRadius(4.f);
            b.shape.setOrigin(4.f, 4.f);
            b.shape.setFillColor(sf::Color::White);
            b.shape.setPosition(b.pos);

            _bullets.push_back(b);

            // Put the turret on cooldown and slightly brighten it as "firing" feedback
            t.cooldown = fireInterval;
            t.shape.setFillColor(sf::Color(0, 230, 255));
        }
        else {
            // No target in range this frame, use idle colour
            t.shape.setFillColor(sf::Color(0, 200, 255));
        }
    }

    // ---- Strip out enemies that died from turret fire ----
    std::vector<Enemy> alive;
    alive.reserve(_enemies.size());
    for (const auto& e : _enemies) {
        if (e.hp > 0) {
            alive.push_back(e);
        }
    }
    _enemies.swap(alive);
}

// Move bullets, check collisions with enemies, and remove expired bullets
void TowerDefenceScene::update_bullets(float dt) {
    if (_bullets.empty()) return;

    std::vector<Bullet> aliveBullets;
    aliveBullets.reserve(_bullets.size());

    for (auto& b : _bullets) {
        // Age the bullet and cull if its time-to-live has expired
        b.ttl -= dt;
        if (b.ttl <= 0.f) continue;

        // Move bullet along its velocity vector
        b.pos += b.vel * b.speed * dt;
        b.shape.setPosition(b.pos);

        bool hit = false;

        // Check for collision against each enemy (simple circle vs circle overlap)
        for (auto& e : _enemies) {
            if (e.hp <= 0) continue; // dead enemies don't collide

            sf::Vector2f d = e.shape.getPosition() - b.pos;
            float r = e.shape.getRadius() + b.shape.getRadius();

            // Compare squared distance to squared combined radii
            if ((d.x * d.x + d.y * d.y) <= r * r) {
                // Bullet has hit this enemy
                e.hp -= b.damage;
                e.flashTimer = 0.2f;   // trigger TD hit flash
                hit = true;
                break;                 // bullet can only hit one enemy
            }
        }

        // Only keep the bullet if it didn't hit anything
        if (!hit) {
            aliveBullets.push_back(b);
        }
    }

    // Replace bullet list with only the ones still alive
    _bullets.swap(aliveBullets);

    // ---- Clean out enemies that died from bullet hits ----
    std::vector<Enemy> alive;
    alive.reserve(_enemies.size());
    for (const auto& e : _enemies) {
        if (e.hp > 0) alive.push_back(e);
    }
    _enemies.swap(alive);
}

// Return and clear list of types of enemies that reached the end of the path
std::vector<int> TowerDefenceScene::consume_escaped_enemies() {
    // SafehouseScene calls this each frame to request "who escaped this tick".
    // We return the current list and then clear it so we don't respawn the same
    // enemies again on the next frame.
    auto result = _escapedEnemyTypes;
    _escapedEnemyTypes.clear();
    return result;
}

void TowerDefenceScene::update(const float& dt) {
    // Update the player entity in TD scene (movement, clamping, hit flash, etc.)
    Scene::update(dt);

    // Swap back to Safehouse with Shift (LShift or RShift)
    if (keyPressedOnce(sf::Keyboard::LShift) || keyPressedOnce(sf::Keyboard::RShift)) {
        GameSystem::set_active_scene(Scenes::safehouse);
        return;
    }

    // Place a turret on the player's current tile with F
    if (keyPressedOnce(sf::Keyboard::F)) {
        place_turret();
    }

    // Run the TD simulation every frame while in this scene:
    //  - enemy spawning
    //  - enemies moving along the path
    //  - turrets firing
    //  - bullets flying and dealing damage
    tick_simulation(dt);
}

void TowerDefenceScene::render(sf::RenderWindow& window) {
    // Background colour for TD scene
    window.draw(_background);

    // Draw the tile grid (walls, path, etc.)
    ls::render(window);

    // Draw the player (from Scene base class)
    Scene::render(window);

    // Draw turrets, bullets, and enemies
    for (const auto& turret : _turrets) window.draw(turret.shape);
    for (const auto& b : _bullets)      window.draw(b.shape);
    for (const auto& enemy : _enemies)  window.draw(enemy.shape);

    // Scene label at top-left
    window.draw(_label);
}

// ============================================================================
// EndScene (simple game-over screen)
// ============================================================================

void EndScene::load() {
    // Load font and configure "Game Over" text
    if (_font.loadFromFile("res/fonts/arial.ttf")) {
        _win_text.setFont(_font);
        _win_text.setString("GAME OVER\nPress R to restart");
        _win_text.setCharacterSize(36);
        _win_text.setFillColor(sf::Color::White);

        // Roughly centred (hard-coded for now)
        _win_text.setPosition(200.f, 200.f);
    }
}

void EndScene::update(const float& dt) {
    // No entities to update, so just handle input
    (void)dt; // silence unused warning if any

    // Press R to restart a fresh run
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
        // Recreate the core scenes from scratch.
        // Note: MazeScene is currently unused in this restart logic –
        // we go straight back to Safehouse + TowerDefence.
        Scenes::safehouse = std::make_shared<SafehouseScene>();
        Scenes::tower_defence = std::make_shared<TowerDefenceScene>();

        // Jump back to the start of the run (Safehouse)
        GameSystem::set_active_scene(Scenes::safehouse);
    }
}

void EndScene::render(sf::RenderWindow& window) {
    // Only draw if we have text set up
    if (_win_text.getString().isEmpty()) return;
    window.draw(_win_text);
}
