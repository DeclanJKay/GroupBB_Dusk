#include "scenes.hpp"
#include "player.hpp"
#include "tile_level_loader/level_system.hpp"
#include "game_parameters.hpp"
#include "game_systems.hpp"
#include "unordered_map"
#include <iostream>

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

    _player = std::make_shared<Player>();
    // Place player somewhere off the lane – e.g. top-left area
    _player->set_position({ 150.f, 100.f });

    _entities.push_back(_player);
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

    // Check tile type – only allow EMPTY
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
    turret.shape.setSize({ tileSize, tileSize });
    turret.shape.setPosition(ls::get_tile_position(grid));
    turret.shape.setFillColor(sf::Color(0, 200, 255)); // cyan-ish

    _turrets.push_back(turret);
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

    // Later:
    // - Update enemies following the path
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
