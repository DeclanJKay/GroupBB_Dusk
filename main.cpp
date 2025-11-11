#include "game_parameters.hpp"
#include "game_systems.hpp"
#include "scenes.hpp"
#include "run_context.hpp"

using param = Parameters;

int main() {
	// Shared run state for this playthrough (e.g. wave number, player stats)
    Scenes::runContext = std::make_shared<RunContext>();

	// This is the old maze scene from before (we added safehouse / tower defence)
    Scenes::maze = std::make_shared<MazeScene>();
    std::static_pointer_cast<MazeScene>(Scenes::maze)->set_file_path(param::maze_1);

	// New scenes for Dusk (safehouse, tower defence, end)
    Scenes::safehouse = std::make_shared<SafehouseScene>();
    Scenes::tower_defence = std::make_shared<TowerDefenceScene>();
    Scenes::end = std::make_shared<EndScene>();

    // Start the game in the safehouse (will change it later to be main menu etc) 
    GameSystem::set_active_scene(Scenes::safehouse);

    GameSystem::start(
        param::game_width,
        param::game_height,
        "Dusk",
        param::time_step
    );

    return 0;
}
