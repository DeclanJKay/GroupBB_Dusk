#pragma once
// Game loop + Scene management (refactored per lab)

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>

class Entity; // forward decl to avoid circular includes

// -------------------------
// Scene base class
// -------------------------
class Scene {
public:
    Scene() = default;
    virtual ~Scene() = default;

    // Called each frame
    virtual void update(const float& dt);
    virtual void render(sf::RenderWindow& window);

    // Lifecycle
    virtual void load() = 0;  // each scene must implement its own load
    virtual void unload();    // default: clear entities

    // Access to scene entities
    std::vector<std::shared_ptr<Entity>>& get_entities() { return _entities; }

protected:
    std::vector<std::shared_ptr<Entity>> _entities;
};

// -------------------------
// GameSystem - static helper
// -------------------------
class GameSystem {
public:
    GameSystem() = delete;

    static void start(unsigned int width,
        unsigned int height,
        const std::string& name,
        const float& time_step = 0.0f);

    static void clean();
    static void reset();
    static void set_active_scene(const std::shared_ptr<Scene>& act_sc);

private:
    static void _init();
    static void _update(const float& dt);
    static void _render(sf::RenderWindow& window);

    static std::shared_ptr<Scene> _active_scene;
};
