#include "scenes.hpp"
#include "player.hpp"
#include "tile_level_loader/level_system.hpp"
#include "game_parameters.hpp"
#include "game_systems.hpp"
#include "unordered_map"
#include <iostream>
#include <cmath>

using ls = LevelSystem;

using param = Parameters;

std::shared_ptr<Scene>      Scenes::maze = nullptr;
std::shared_ptr<Scene>      Scenes::safehouse = nullptr;
std::shared_ptr<Scene>      Scenes::tower_defence = nullptr;
std::shared_ptr<Scene>      Scenes::end = nullptr;
std::shared_ptr<RunContext> Scenes::runContext = nullptr;


void MazeScene::set_file_path(const std::string& file_path) {
    _file_path = file_path;
}

// Utility to detect a single key press so we can easily swap between tdf/action scenes (edge trigger)
bool keyPressedOnce(sf::Keyboard::Key key) {
    static std::unordered_map<sf::Keyboard::Key, bool> keyStates;

    bool isPressed = sf::Keyboard::isKeyPressed(key);
    bool wasPressed = keyStates[key];

    keyStates[key] = isPressed; // update stored state

    return (isPressed && !wasPressed); // true only on first press
}


void MazeScene::load() {
    // Colors 
    ls::set_color(ls::EMPTY, sf::Color(30, 30, 30));
    ls::set_color(ls::WALL, sf::Color(180, 180, 180));
    ls::set_color(ls::START, sf::Color(80, 255, 80));
    ls::set_color(ls::END, sf::Color(255, 80, 80));

    // Player entity
    auto player = std::make_shared<Player>();
    _entities.push_back(player);

    reset(); // loads the level + positions player
}

void MazeScene::reset() {
    // Load the level and place player on START tile
    ls::load_level(_file_path, 50.f); // choose your tile_size (e.g. 50)
    if (!_entities.empty()) {
        _entities.front()->set_position(ls::get_start_position());
    }
}

void MazeScene::update(const float& dt) {
    // Update entities (includes Player movement)
    Scene::update(dt);

    // If player reached END -> switch scene or next maze
    if (_entities.empty()) return;
    const sf::Vector2f p = _entities.front()->get_position();

    try {
        if (ls::get_tile_at(p) == ls::END) {
            // Switch to second maze if on first, otherwise go to end scene
            if (_file_path == std::string(param::maze_1)) {
                _file_path = param::maze_2;
                reset();
                return;
            }
            // finished second maze -> go to end screen
            unload();                                  // unload the maze scene
            GameSystem::set_active_scene(Scenes::end); // switch to end scene
            return;

        }
    }
    catch (...) {
        // ignore OOB
    }
}

void MazeScene::render(sf::RenderWindow& window) {
    ls::render(window);       // draw the tiles first
    Scene::render(window);    // then draw entities (player)


}

// -------------------------
// SafehouseScene
// -------------------------

void SafehouseScene::load() {
    _background.setSize({
        static_cast<float>(param::game_width),
        static_cast<float>(param::game_height)
        });

	// colour to distinguish from tower defence
    _background.setFillColor(sf::Color(30, 15, 15));

	// Load font and set up a lavel (so we can see we are in safehouse)
    if (!_font.loadFromFile("res/fonts/ARIAL.TTF")) {
        std::cerr << "Failed to load font: res/fonts/ARIAL.TTF\n";
    }

    _label.setFont(_font);
    _label.setString("SAFEHOUSE");
    _label.setCharacterSize(32);
    _label.setFillColor(sf::Color::White);
    _label.setPosition(20.f, 20.f);

    //create a player for this scene
    _entities.clear();

    auto player = std::make_shared<Player>();
    // start roughly in the centre of the screen
    player->set_position({
        param::game_width * 0.5f,
        param::game_height * 0.5f
        });

    _entities.push_back(player);

    // Later on we will put other info like:
    // - Spawn player entity
    // - Place vendor / doors
    // - Setup walls / collision
	// - etc.
}

void SafehouseScene::update(const float& dt) {
    // Update any entities managed by the base Scene
    Scene::update(dt);

    // Press Shift once to swap to tower defence view
    if (keyPressedOnce(sf::Keyboard::LShift) || keyPressedOnce(sf::Keyboard::RShift)) {
        GameSystem::set_active_scene(Scenes::tower_defence);
        return;
    }

    // Later:
    // - Handle player movement / attacks
    // - Handle shop interaction
    // - Check for run end and set Scenes::runContext->runOver
	// - etc.
}

void SafehouseScene::render(sf::RenderWindow& window) {
    window.draw(_background);
	Scene::render(window); // draw player
    window.draw(_label);
    // Later: draw player, UI, vendor, etc.
}


// -------------------------
// TowerDefenceScene
// -------------------------

void TowerDefenceScene::load() {
    // Optional: background tint (can be very dark so tiles still show)
    _background.setSize({
        static_cast<float>(param::game_width),
        static_cast<float>(param::game_height)
        });
    _background.setFillColor(sf::Color(5, 5, 20));

    // Set tile colours for this scene
    ls::set_color(ls::EMPTY, sf::Color(10, 10, 30));      // dark floor
    ls::set_color(ls::WALL, sf::Color(60, 60, 80));      // walls/border
    ls::set_color(ls::WAYPOINT, sf::Color(120, 120, 120));   // enemy lane
    ls::set_color(ls::START, sf::Color(80, 255, 80));     // if you add 's' later
    ls::set_color(ls::END, sf::Color(255, 80, 80));     // if you add 'e' later

    // Load TD map (uses w / space / +)
    const float tileSize = 50.f;
    ls::load_level(param::td_1, 50.f);  // 50.f tile size like the maze

    // Label
    if (!_font.loadFromFile("res/fonts/ARIAL.TTF")) {
        std::cerr << "Failed to load font: res/fonts/ARIAL.TTF\n";
    }

    _label.setFont(_font);
    _label.setString("TOWER DEFENCE");
    _label.setCharacterSize(32);
    _label.setFillColor(sf::Color::White);
    _label.setPosition(20.f, 20.f);

    // Player for this scene
    _entities.clear();
    _turrets.clear();
    _enemies.clear();
    _bullets.clear();
    _enemyPath.clear();
    _spawnTimer = 0.f;

    _player = std::make_shared<Player>();
    // Place player somewhere off the lane
    _player->set_position({ 150.f, 100.f });

    _entities.push_back(_player);

    // Build the ordered enemy path from the + tiles
    build_enemy_path();
}

void TowerDefenceScene::build_enemy_path() {
    _enemyPath.clear();

    const int w = ls::get_width();
    const int h = ls::get_height();
    const float tileSize = 50.f; // must match load_level

    // Collect all WAYPOINT tiles
    std::vector<sf::Vector2i> waypoints;
    waypoints.reserve(w * h);

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

    // Find a start tile: choose the leftmost (+ top-most) waypoint
    sf::Vector2i start = waypoints[0];
    for (const auto& p : waypoints) {
        if (p.x < start.x || (p.x == start.x && p.y < start.y)) {
            start = p;
        }
    }

    // Simple snake follow: walk from start, always going to an adjacent
    // unvisited WAYPOINT tile (assumes a single, non-branching path)
    auto index = [w](sf::Vector2i p) {
        return p.y * w + p.x;
        };

    std::vector<bool> visited(static_cast<size_t>(w * h), false);
    std::vector<sf::Vector2i> ordered;
    ordered.reserve(waypoints.size());

    sf::Vector2i current = start;
    ordered.push_back(current);
    visited[static_cast<size_t>(index(current))] = true;

    const sf::Vector2i dirs[4] = {
        { 1,  0 },
        { -1, 0 },
        { 0,  1 },
        { 0, -1 }
    };

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

    // Convert grid positions to world positions (centre of each tile)
    _enemyPath.reserve(ordered.size());
    for (const auto& grid : ordered) {
        sf::Vector2f tilePos = ls::get_tile_position(grid);
        _enemyPath.push_back(tilePos + sf::Vector2f(tileSize * 0.5f, tileSize * 0.5f));
    }

    std::cout << "Enemy path built with " << _enemyPath.size() << " nodes.\n";
}

void TowerDefenceScene::spawn_enemy() {
    if (_enemyPath.size() < 2) return;

    Enemy e;
    e.t = 0.f; // start at beginning of path
    e.hp = 3;  //takes 3 hits to kill enemy (will change later)

    e.shape.setRadius(15.f);
    e.shape.setOrigin(15.f, 15.f);
    e.shape.setFillColor(sf::Color::Red);
    e.shape.setPosition(_enemyPath.front());

    _enemies.push_back(e);
}

void TowerDefenceScene::update_enemies(float dt) {
    if (_enemyPath.size() < 2) return;

    const float tileSize = 50.f;
    const float speed = 60.f; // units per second along the path

    // Move enemies along the path
    for (auto& e : _enemies) {
        // Convert speed to progress along path nodes
        float deltaT = (speed * dt) / tileSize;
        e.t += deltaT;

        int segment = static_cast<int>(e.t);
        float local = e.t - segment;

        if (segment >= static_cast<int>(_enemyPath.size()) - 1) {
            // Enemy reached end of path; for now just park it at the last node
            e.shape.setPosition(_enemyPath.back());
        }
        else {
            const sf::Vector2f& a = _enemyPath[static_cast<size_t>(segment)];
            const sf::Vector2f& b = _enemyPath[static_cast<size_t>(segment + 1)];
            e.shape.setPosition(a + (b - a) * local);
        }

        if (e.flashTimer > 0.f) {
            e.flashTimer -= dt;
            float t = std::max(e.flashTimer / 0.2f, 0.f);
            sf::Color c;
            c.r = static_cast<sf::Uint8>(255);
            c.g = static_cast<sf::Uint8>(180 + 75 * t);
            c.b = static_cast<sf::Uint8>(180 + 75 * t);
            e.shape.setFillColor(c);
        }
        else {
            e.shape.setFillColor(sf::Color::Red);
        }
    }

    // Remove enemies that reached the end (segment beyond path)
    std::vector<Enemy> alive;
    alive.reserve(_enemies.size());
    for (const auto& e : _enemies) {
        int segment = static_cast<int>(e.t);
        if (segment < static_cast<int>(_enemyPath.size()) - 1) {
            alive.push_back(e);
        }
        // else: enemy finished path; later we'll damage base etc.
    }
    _enemies.swap(alive);
}



void TowerDefenceScene::place_turret() {
    if (!_player) return;

    // Must match the tile size used in ls::load_level above
    const float tileSize = 50.f;

    sf::Vector2f pos = _player->get_position();
    sf::Vector2i grid(
        static_cast<int>(pos.x / tileSize),
        static_cast<int>(pos.y / tileSize)
    );

    // Check tile type � only allow EMPTY
    LevelSystem::Tile tile;
    try {
        tile = ls::get_tile(grid);
    }
    catch (...) {
        // out of bounds, do nothing
        return;
    }

    if (tile != ls::EMPTY) {
        // can't place on walls, lane, enemy, etc.
        return;
    }

    // Prevent placing multiple turrets on the same tile
    for (const auto& t : _turrets) {
        if (t.grid == grid) {
            return; // turret already here
        }
    }

    // Create visual turret square
    Turret turret;
    turret.grid = grid;
    turret.cooldown = 0.f;
    turret.shape.setSize({ tileSize, tileSize });
    turret.shape.setPosition(ls::get_tile_position(grid));
    turret.shape.setFillColor(sf::Color(0, 200, 255)); // cyan-ish colour 

    _turrets.push_back(turret);

}


void TowerDefenceScene::update_turrets(float dt) {
    if (_enemies.empty() || _turrets.empty()) return;

    const float tileSize = 50.f;          // must match load_level
    const float rangePixels = tileSize * 3;  // 3-tile range
    const float rangeSq = rangePixels * rangePixels;
    const float fireInterval = 0.5f;          // seconds between shots
    const int   damagePerHit = 1;

    // For each turret, try to shoot one enemy
    for (auto& t : _turrets) {
        // tick down cooldown
        if (t.cooldown > 0.f) {
            t.cooldown -= dt;
            continue;
        }

        // world position of turret centre
        sf::Vector2f turretPos =
            t.shape.getPosition() + sf::Vector2f(tileSize * 0.5f, tileSize * 0.5f);

        Enemy* bestEnemy = nullptr;
        float bestDistSq = rangeSq;

        for (auto& e : _enemies) {
            if (e.hp <= 0) continue; // already dead

            sf::Vector2f diff = e.shape.getPosition() - turretPos;
            float d2 = diff.x * diff.x + diff.y * diff.y;

            if (d2 < bestDistSq) {
                bestDistSq = d2;
                bestEnemy = &e;
            }
        }

        if (bestEnemy) {
            // Fire a projectile toward the chosen enemy
            sf::Vector2f turretPos =
                t.shape.getPosition() + sf::Vector2f(25.f, 25.f); // tileSize*0.5

            Bullet b;
            b.pos = turretPos;
            sf::Vector2f toEnemy = bestEnemy->shape.getPosition() - turretPos;
            float len = std::sqrt(toEnemy.x * toEnemy.x + toEnemy.y * toEnemy.y);
            b.vel = (len > 0.f) ? (toEnemy / len) : sf::Vector2f(0.f, 0.f);
            b.speed = 300.f;     // tweak as you like
            b.damage = 1;
            b.ttl = 2.0f;

            b.shape.setRadius(4.f);
            b.shape.setOrigin(4.f, 4.f);
            b.shape.setFillColor(sf::Color::White);
            b.shape.setPosition(b.pos);

            _bullets.push_back(b);

            // fire rate + cosmetic flash
            t.cooldown = fireInterval;
            t.shape.setFillColor(sf::Color(0, 230, 255));

        }
        else {
            // No enemy in range: slowly fade back toward base colour
            t.shape.setFillColor(sf::Color(0, 200, 255));
        }
    }

    // Remove dead enemies (hp <= 0), but keep those still walking
    std::vector<Enemy> alive;
    alive.reserve(_enemies.size());
    for (const auto& e : _enemies) {
        if (e.hp > 0) {
            alive.push_back(e);
        }
    }
    _enemies.swap(alive);
}

void TowerDefenceScene::update_bullets(float dt) {
    if (_bullets.empty()) return;

    // Move bullets, check collisions, cull
    std::vector<Bullet> aliveBullets;
    aliveBullets.reserve(_bullets.size());

    for (auto& b : _bullets) {
        b.ttl -= dt;
        if (b.ttl <= 0.f) continue;

        b.pos += b.vel * b.speed * dt;
        b.shape.setPosition(b.pos);

        bool hit = false;

        // collision check vs enemies (circle-circle)
        for (auto& e : _enemies) {
            if (e.hp <= 0) continue;
            sf::Vector2f d = e.shape.getPosition() - b.pos;
            float r = e.shape.getRadius() + b.shape.getRadius();
            if ((d.x * d.x + d.y * d.y) <= r * r) {
                e.hp -= b.damage;
                e.flashTimer = 0.2f; //flash for 0.2 seconds
                hit = true;
                break;
            }
        }

        if (!hit) {
            aliveBullets.push_back(b);
        }
    }
    _bullets.swap(aliveBullets);

    // remove dead enemies
    std::vector<Enemy> alive;
    alive.reserve(_enemies.size());
    for (const auto& e : _enemies) {
        if (e.hp > 0) alive.push_back(e);
    }
    _enemies.swap(alive);
}




void TowerDefenceScene::update(const float& dt) {
    Scene::update(dt);

    // Press Shift once to go back to safehouse view
    if (keyPressedOnce(sf::Keyboard::LShift) || keyPressedOnce(sf::Keyboard::RShift)) {
        GameSystem::set_active_scene(Scenes::safehouse);
        return;
    }

    // F  place turret on current tile (if valid)
    if (keyPressedOnce(sf::Keyboard::F)) {
        place_turret();
    }

    // Enemy spawning
    if (!_enemyPath.empty()) {
        _spawnTimer += dt;
        const float spawnInterval = 2.0f; // one enemy every 2 seconds

        if (_spawnTimer >= spawnInterval) {
            _spawnTimer -= spawnInterval;
            spawn_enemy();
        }
    }

    // Enemy movement along the path
    update_enemies(dt);
    //turrets fire 
    update_turrets(dt);
    update_bullets(dt);

    // Later:
    // - Towers targeting and firing
    // - Wave completion and rewards
}

void TowerDefenceScene::render(sf::RenderWindow& window) {
    window.draw(_background);
    ls::render(window); //draw the tile map
	Scene::render(window); // draw player

    // Turrets on top of tiles
    for (const auto& turret : _turrets) {
        window.draw(turret.shape);
    }

    // Bullets
    for (const auto& b : _bullets) window.draw(b.shape);

    // Enemies
    for (const auto& enemy : _enemies) {
        window.draw(enemy.shape);
    }

    window.draw(_label);
    // Later: draw turrets, enemies, bullets, wave UI, etc.
}



void EndScene::load() {
    // NOTE: put a font at res/fonts/arial.ttf (or change the path)
    if (_font.loadFromFile("res/fonts/arial.ttf")) {
        _win_text.setFont(_font);
        _win_text.setString("You Win!\nPress ESC to exit.");
        _win_text.setCharacterSize(36);
        _win_text.setFillColor(sf::Color::White);

        // center it roughly
        _win_text.setPosition(200.f, 200.f);
    }
}

void EndScene::render(sf::RenderWindow& window) {
    if (_win_text.getString().isEmpty()) return;
    window.draw(_win_text);
}
