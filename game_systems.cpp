// Implementation of Scene + GameSystem

#include "game_systems.hpp"
#include "entity.hpp" // needed for update/render calls

#include <SFML/Graphics.hpp>

// -------------------------
// Static storage
// -------------------------
std::shared_ptr<Scene> GameSystem::_active_scene = nullptr;

// -------------------------
// Scene impl
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
    // Default behavior: drop all entities
    _entities.clear();
}

// -------------------------
// GameSystem impl
// -------------------------
void GameSystem::start(unsigned int width,
    unsigned int height,
    const std::string& name,
    const float& time_step)
{
    sf::RenderWindow window(sf::VideoMode({ width, height }), name);
    window.setFramerateLimit(0); // we control pacing (sleep) ourselves

    _init();

    sf::Event event{};
    sf::Clock clock;

    while (window.isOpen()) {
        // OS events
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                clean();
                return;
            }
        }

        // Quick exit for labs
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
            break;
        }

        const float dt = clock.restart().asSeconds();

        window.clear();
        _update(dt);
        _render(window);

        // Crude pacing to emulate VSync per lab text (optional)
        if (time_step > 0.0f) {
            sf::sleep(sf::seconds(time_step));
        }
        window.display();
    }

    window.close();
    clean();
}

void GameSystem::clean() {
    if (_active_scene) {
        _active_scene->unload();
        _active_scene.reset();
    }
}

void GameSystem::reset() {
    if (_active_scene) {
        _active_scene->unload();
        _active_scene->load();
    }
}

void GameSystem::set_active_scene(const std::shared_ptr<Scene>& act_sc) {
    if (_active_scene) _active_scene->unload();
    _active_scene = act_sc;
    if (_active_scene) _active_scene->load();
}

void GameSystem::_init() {
    // One-time setup point if you need global managers later.
}

void GameSystem::_update(const float& dt) {
    if (_active_scene) _active_scene->update(dt);
}

void GameSystem::_render(sf::RenderWindow& window) {
    if (_active_scene) _active_scene->render(window);
}
