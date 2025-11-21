// game_systems.cpp
// Implements Scene base behaviour and the global GameSystem loop.

#include "game_systems.hpp"
#include "entity.hpp" // needed for update/render calls

#include <SFML/Graphics.hpp>

// -------------------------
// Static storage
// -------------------------
std::shared_ptr<Scene> GameSystem::_active_scene = nullptr;
// Single global window owned by GameSystem
std::unique_ptr<sf::RenderWindow> GameSystem::_window = nullptr;

// -------------------------
// Scene implementation
// -------------------------

void Scene::update(const float& dt) {
    // Update all entities in this scene
    for (auto& ent : _entities) {
        if (ent) ent->update(dt);
    }
}

void Scene::render(sf::RenderWindow& window) {
    // Draw all entities in this scene
    for (auto& ent : _entities) {
        if (ent) ent->render(window);
    }
}

void Scene::unload() {
    // Default behaviour: clear all entities
    _entities.clear();
}

// -------------------------
// GameSystem implementation
// -------------------------

void GameSystem::start(unsigned int width,
    unsigned int height,
    const std::string& name,
    const float& time_step)
{
    // Create and own the main render window
    _window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode({ width, height }),
        name
    );
    sf::RenderWindow& window = *_window;
    window.setFramerateLimit(0); // we control pacing manually

    _init();

    sf::Event event{};
    sf::Clock clock;

    // Core game loop
    while (window.isOpen()) {
        // Handle OS / window events
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                clean();
                return;
            }
        }

        // Quick exit during development
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
            break;
        }

        // Time since last frame
        const float dt = clock.restart().asSeconds();

        window.clear();
        _update(dt);
        _render(window);

        // Optional fixed pacing (acts like a manual vsync)
        if (time_step > 0.0f) {
            sf::sleep(sf::seconds(time_step));
        }
        window.display();
    }

    window.close();
    clean();
}

sf::RenderWindow& GameSystem::get_window() {
    // Accessor for the global window (used e.g. for mouse coords)
    return *_window;
}

void GameSystem::clean() {
    // Unload and forget current scene
    if (_active_scene) {
        _active_scene->unload();
        _active_scene.reset();
    }
}

void GameSystem::reset() {
    // Reload the current scene from scratch
    if (_active_scene) {
        _active_scene->unload();
        _active_scene->load();
    }
}

void GameSystem::set_active_scene(const std::shared_ptr<Scene>& act_sc) {
    // Swap scenes, calling unload/load around the change
    if (_active_scene) _active_scene->unload();
    _active_scene = act_sc;
    if (_active_scene) _active_scene->load();
}

void GameSystem::_init() {
    // One-time setup point for global managers if needed
}

void GameSystem::_update(const float& dt) {
    // Forward update to the active scene
    if (_active_scene) _active_scene->update(dt);
}

void GameSystem::_render(sf::RenderWindow& window) {
    // Forward render to the active scene
    if (_active_scene) _active_scene->render(window);
}
