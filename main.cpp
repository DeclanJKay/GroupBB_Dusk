#include "game_parameters.hpp"
#include "game_systems.hpp"
#include "scenes.hpp"
#include "run_context.hpp"

using param = Parameters;

int main() {
    // Shared run state for this playthrough (e.g. wave number, player stats)
    Scenes::runContext = std::make_shared<RunContext>();

    // Core Dusk scenes
    Scenes::safehouse = std::make_shared<SafehouseScene>();
    Scenes::tower_defence = std::make_shared<TowerDefenceScene>();
    Scenes::end = std::make_shared<EndScene>();

    // Start the game in the safehouse (later this could be a main menu)
    GameSystem::set_active_scene(Scenes::safehouse);

    // Kick off the main game loop
    GameSystem::start(
        param::game_width,
        param::game_height,
        "Dusk",
        param::time_step
    );

    return 0;
}
