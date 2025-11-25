// game_systems.cpp
// Implements Scene base behaviour and the global GameSystem loop.

#include <iostream>
#include "game_systems.hpp"
#include "entity.hpp" // needed for update/render calls

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

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

b2WorldId worldId;
sf::RectangleShape groundShape;
sf::RectangleShape testShape;
b2BodyId groundId;
b2BodyId testId;

Goon yes;

static EntType typeArray[(int)count];


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

void GameSystem::_init() 
{
    // One-time setup point for global managers if needed
    EntityTags::PopulateArray();


    b2WorldDef wDef = b2DefaultWorldDef();
    wDef.gravity = (b2Vec2){0.0f, -10.0f};
    worldId = b2CreateWorld(&wDef);

    /*
    groundShape.setSize(sf::Vector2f(800.f,32.f));
    groundShape.setOrigin(400.f, 16.f);
    groundShape.setFillColor(sf::Color::White);

    b2BodyDef gdef = b2DefaultBodyDef();
    gdef.position = (b2Vec2){12.5f, 0.00f};
    gdef.userData = &typeArray[(int)player];
    groundId = b2CreateBody(worldId, &gdef);
    b2Polygon gBox = b2MakeBox(12.5f, 0.5f);
    b2ShapeDef gshapedef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &gshapedef, &gBox);

    testShape.setSize(sf::Vector2f(32.f,32.f));
    testShape.setOrigin(16.f, 16.f);
    testShape.setFillColor(sf::Color::White);

    b2BodyDef tdef = b2DefaultBodyDef();
    tdef.type = b2_dynamicBody;
    tdef.position = (b2Vec2){10.0f, 10.0f};
    testId = b2CreateBody(worldId, &tdef);
    b2Polygon tBox = b2MakeBox(0.5f,0.5f);
    b2ShapeDef tshapedef = b2DefaultShapeDef();
    tshapedef.density = 1.0f;
    tshapedef.enableHitEvents = true;
    tshapedef.material.friction = 0.3f;
    b2CreatePolygonShape(testId, &tshapedef, &tBox);
    */

    yes = Goon(&worldId);

    if(EntityTags::CheckForTag(yes.getBodyID(), player)){std::cout<<"yes";}
}

void GameSystem::_update(const float& dt) {
    // Forward update to the active scene
    if (_active_scene) _active_scene->update(dt);
    b2World_Step(worldId, 1.f/60.f, 4);

    auto events = b2World_GetContactEvents(worldId);
    
    /*
    b2Vec2 dir = b2Vec2{0,0};
    EntType* iptr = (EntType*)b2Body_GetUserData(groundId);
    //if (*iptr == player){std::cout<<"hooray";}

    // Basic WASD / Arrow movement input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) dir.x += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  dir.y += 1.f;

    if (dir.x != 0 || dir.y != 0 ) {b2Body_ApplyLinearImpulseToCenter(testId, dir, true);}

    for (int i = 0; i < events.hitCount; i++)
    {
        auto &ev = events.hitEvents[i];
        auto bod1 = b2Shape_GetBody(ev.shapeIdA);
        auto bod2 = b2Shape_GetBody(ev.shapeIdB);
    }
        */
}

void GameSystem::_render(sf::RenderWindow& window) {
    // Forward render to the active scene
    if (_active_scene) _active_scene->render(window);

    //auto tPos = b2Body_GetPosition(testId);
    //testShape.setPosition(tPos.x*32.f, tPos.y*32.f);

    //auto gPos = b2Body_GetPosition(groundId);
    //groundShape.setPosition(gPos.x*32.f, gPos.y*32.f);

    //window.draw(testShape);
    //window.draw(groundShape);
    yes.RenderHitbox(window, sf::Color::Green);
}
