#include "scenes.hpp"
#include "player.hpp"
#include "tile_level_loader/level_system.hpp"
#include "game_parameters.hpp"
#include "TDEnemy.hpp"
#include "EnemyStats.hpp"


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

    // NEW: Wave / level text
    _waveText.setFont(_font);
    _waveText.setCharacterSize(24);
    _waveText.setFillColor(sf::Color::White);
    _waveText.setPosition(20.f, 90.f);   // just below HP text
    _waveText.setString("Level 1 - Wave 1/5");

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

void SafehouseScene::tick_simulation(float dt) {
    // If we’ve never been loaded / initialised, nothing to do
    if (!_initialised || !_player) return;

    // Cooldown for contact damage still counts down
    if (_damageCooldown > 0.f) {
        _damageCooldown -= dt;
        if (_damageCooldown < 0.f) _damageCooldown = 0.f;
    }

    // Move invaders and apply contact damage, same as in update()
    update_invaders(dt);

    // Optional: if player can die off-screen, keep this
    if (_player->is_dead()) {
        _invaders.clear();

        if (!Scenes::end) {
            Scenes::end = std::make_shared<EndScene>();
        }

        GameSystem::set_active_scene(Scenes::end);
    }
}


void SafehouseScene::spawn_invaders(const std::vector<int>& enemyTypes) {
    for (int typeId : enemyTypes) {
        Invader inv;

        // Convert int -> EnemyType
        EnemyType type = static_cast<EnemyType>(typeId);

        // Look up shared stats
        EnemyStats stats = get_enemy_stats(type);

        inv.speed = stats.speed;
        inv.hp = stats.hp;
        inv.maxHp = stats.hp;
        inv.baseColor = stats.color;

        inv.shape.setRadius(stats.radius);
        inv.shape.setOrigin(stats.radius, stats.radius);
        inv.shape.setFillColor(stats.color);

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

    // --- Wave / Level UI (query TowerDefenceScene) ---
    if (Scenes::tower_defence) {
        auto td = std::static_pointer_cast<TowerDefenceScene>(Scenes::tower_defence);

        if (!td->hasFinishedAllWaves()) {
            int levelIdx = td->getCurrentLevelIndex() + 1; // 0-based -> 1-based
            int waveIdx = td->getCurrentWaveIndex() + 1;
            int totalWaves = td->getWavesInCurrentLevel();

            std::string extra;
            if (td->isWaitingForPlayer()) {
                extra = "  (Press E in TD to start)";
            }

            _waveText.setString(
                "Level " + std::to_string(levelIdx) +
                " - Wave " + std::to_string(waveIdx) +
                "/" + std::to_string(totalWaves) +
                extra
            );
        }
        else {
            _waveText.setString("All waves complete");
        }
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
    window.draw(_waveText);
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

    // Wave UI text
    _waveText.setFont(_font);
    _waveText.setCharacterSize(24);
    _waveText.setFillColor(sf::Color::White);
    _waveText.setPosition(20.f, 60.f);
    _waveText.setString("Wave 0/0");

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
        _escapedEnemyTypes.clear();

        // Build the path (+ tiles) enemies will follow
        build_enemy_path();

        // Reset wave manager at the start of a new run / level
        _waveManager.reset();

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
void TowerDefenceScene::tick_simulation(float dt) {
    if (_enemyPath.empty()) return;

    // 1) WaveManager handles spawning when an active wave is running
    _waveManager.update(
        dt,
        static_cast<int>(_enemies.size()),
        [this](EnemyType type)
        {
            // When WaveManager wants a new enemy, spawn it at the start of the path
            const sf::Vector2f startPos = _enemyPath.front();
            _enemies.emplace_back(type, startPos);
        }
    );

    // 2) Normal TD simulation
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


void TowerDefenceScene::update_enemies(float dt) {
    if (_enemyPath.size() < 2) return;

    const float tileSize = 50.f;

    std::vector<TDEnemy> alive;
    alive.reserve(_enemies.size());

    for (auto& enemy : _enemies) {
        // Skip enemies that have already been killed by turrets/bullets
        if (enemy.isDead()) {
            continue;
        }

        // Let TDEnemy handle movement + flashing
        bool reachedEnd = enemy.update(dt, _enemyPath, tileSize);

        if (reachedEnd) {
            // Tell the Safehouse what type escaped
            _escapedEnemyTypes.push_back(static_cast<int>(enemy.getType()));
        }
        else {
            alive.push_back(enemy);
        }
    }

    _enemies.swap(alive);
}


void TowerDefenceScene::place_turret() {
    if (!_player) return;

    const float tileSize = 50.f;

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
        return;
    }

    // Only allow placing on EMPTY tiles
    if (tile != ls::EMPTY) {
        return;
    }

    // Don’t double-place a turret on the same tile
    for (const auto& t : _turrets) {
        if (t.getGrid() == grid) {
            return;
        }
    }

    // World position of this tile
    sf::Vector2f worldPos = ls::get_tile_position(grid);

    // Create a new turret instance
    _turrets.emplace_back(grid, worldPos, tileSize);
}


// Ask each turret if it wants to fire this frame and spawn bullets
void TowerDefenceScene::update_turrets(float dt) {
    if (_turrets.empty()) return;

    for (auto& t : _turrets) {
        sf::Vector2f bulletPos;
        sf::Vector2f bulletDir;

        // TDTurret handles range, cooldown, target selection.
        // If it returns true, we spawn a bullet.
        if (t.update(dt, _enemies, bulletPos, bulletDir)) {
            _bullets.emplace_back(bulletPos, bulletDir);
        }
    }

    // Clean out any enemies that died from turret damage
    _enemies.erase(
        std::remove_if(
            _enemies.begin(), _enemies.end(),
            [](const TDEnemy& e) { return e.isDead(); }
        ),
        _enemies.end()
    );
}


// Move bullets, apply damage, and cull dead bullets + enemies
void TowerDefenceScene::update_bullets(float dt) {
    if (_bullets.empty()) return;

    std::vector<TDBullet> alive;
    alive.reserve(_bullets.size());

    for (auto& b : _bullets) {
        // TDBullet::update handles movement + collision + enemy damage.
        if (b.update(dt, _enemies)) {
            alive.push_back(b);   // still alive this frame
        }
        // else: bullet expired or hit something -> drop it
    }
    _bullets.swap(alive);

    // Clean out enemies that died from bullet damage
    _enemies.erase(
        std::remove_if(
            _enemies.begin(), _enemies.end(),
            [](const TDEnemy& e) { return e.isDead(); }
        ),
        _enemies.end()
    );
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
    // Update the player in this scene
    Scene::update(dt);

    // Run safehouse simulation in the background too
    if (Scenes::safehouse) {
        auto sh = std::static_pointer_cast<SafehouseScene>(Scenes::safehouse);
        sh->tick_simulation(dt);
    }

    // Handle starting the next wave with E
    if (_waveManager.isWaitingForPlayer() && keyPressedOnce(sf::Keyboard::E)) {
        _waveManager.startNextWave();
    }

    // Swap back to Safehouse with Shift (LShift or RShift)
    if (keyPressedOnce(sf::Keyboard::LShift) || keyPressedOnce(sf::Keyboard::RShift)) {
        GameSystem::set_active_scene(Scenes::safehouse);
        return;
    }

    // Place a turret on the player's current tile with F
    if (keyPressedOnce(sf::Keyboard::F)) {
        place_turret();
    }

    // Run full TD sim (spawning, movement, turrets, bullets)
    tick_simulation(dt);

    // --- Update wave UI text ---
    if (!_waveManager.hasFinishedAllWaves()) {
        int levelIdx = _waveManager.getCurrentLevelIndex() + 1;
        int waveIdx = _waveManager.getCurrentWaveIndex() + 1;
        int totalWaves = _waveManager.getWavesInCurrentLevel();

        std::string extra;
        if (_waveManager.isWaitingForPlayer()) {
            extra = "  (Press E to start)";
        }

        _waveText.setString(
            "Level " + std::to_string(levelIdx) +
            " - Wave " + std::to_string(waveIdx) +
            "/" + std::to_string(totalWaves) +
            extra
        );
    }
    else {
        _waveText.setString("All waves complete");
    }
}


void TowerDefenceScene::render(sf::RenderWindow& window) {
    // Background colour for TD scene
    window.draw(_background);

    // Draw the tile grid (walls, path, etc.)
    ls::render(window);

    // Draw the player (from Scene base class)
    Scene::render(window);

    // Draw turrets, bullets, and enemies
    for (const auto& turret : _turrets) turret.render(window);
    for (const auto& b : _bullets)      b.render(window);
    for (const auto& enemy : _enemies)  window.draw(enemy.getShape());

    // Scene label at top-left
    window.draw(_label);
    window.draw(_waveText);
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
