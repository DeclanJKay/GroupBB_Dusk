#pragma once
// Game loop + Scene management

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>

class Entity; // forward declaration to avoid circular includes

// -------------------------
// Scene base class
// -------------------------
class Scene {
public:
    Scene() = default;
    virtual ~Scene() = default;

    // Called once per frame
    virtual void update(const float& dt);
    virtual void render(sf::RenderWindow& window);

    // Scene lifecycle hooks
    virtual void load() = 0;   // called when the scene becomes active
    virtual void unload();     // default: clear all entities

    // Access to the scene’s entity list
    std::vector<std::shared_ptr<Entity>>& get_entities() { return _entities; }

protected:
    // All entities owned by this scene
    std::vector<std::shared_ptr<Entity>> _entities;
};

// -------------------------
// GameSystem - static helper
// -------------------------
// Owns the main window and runs the core game loop.
// Only one active Scene exists at a time.
class GameSystem {
public:
    GameSystem() = delete; // purely static class

    // Create window and run the main loop
    static void start(unsigned int width,
        unsigned int height,
        const std::string& name,
        const float& time_step = 0.0f);

    // Global access to the SFML window (e.g. for mouse coords)
    static sf::RenderWindow& get_window();

    // Tear down current scene
    static void clean();

    // Reload the current active scene
    static void reset();

    // Change which scene is active (calls unload/load)
    static void set_active_scene(const std::shared_ptr<Scene>& act_sc);

private:
    // Internal helpers for the main loop
    static void _init();
    static void _update(const float& dt);
    static void _render(sf::RenderWindow& window);

    // Currently active scene
    static std::shared_ptr<Scene> _active_scene;

    // Single global window owned by GameSystem
    static std::unique_ptr<sf::RenderWindow> _window;
};
