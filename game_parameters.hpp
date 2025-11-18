// game_parameters.hpp
#pragma once 

// Global game settings and shared paths
struct Parameters {
    // Window size
    static constexpr unsigned int game_width = 800;
    static constexpr unsigned int game_height = 600;

    // Fixed timestep target (60 FPS)
    static constexpr float time_step = 1.0f / 60.0f;

	// Maze level files (will get rid of the maze_ prefix later)
    static constexpr const char* maze_1 = "res/levels/maze_1.txt";
    static constexpr const char* maze_2 = "res/levels/maze_2.txt";

    // Tower defence grid level
    static constexpr const char* td_1 = "res/levels/td_1.txt";
};
