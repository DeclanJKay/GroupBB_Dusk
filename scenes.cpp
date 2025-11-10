#include "scenes.hpp"
#include "player.hpp"
#include "tile_level_loader/level_system.hpp"
#include "game_parameters.hpp"
#include "game_systems.hpp"

#include <iostream>

using ls = LevelSystem;

using param = Parameters;

std::shared_ptr<Scene> Scenes::maze = nullptr;
std::shared_ptr<Scene> Scenes::end = nullptr;

void MazeScene::set_file_path(const std::string& file_path) {
    _file_path = file_path;
}

void MazeScene::load() {
    // Colors (optional tweak)
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
