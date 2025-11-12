//game_parameters.hpp
#pragma once 

struct Parameters {
    static constexpr unsigned int game_width = 800;
    static constexpr unsigned int game_height = 600;
    static constexpr float time_step = 1.0f / 60.0f; // add this
    // inside Parameters (struct or namespace)
    static constexpr const char* maze_1 = "res/levels/maze_1.txt";
    static constexpr const char* maze_2 = "res/levels/maze_2.txt";

    // This is the tower defence gird level 
    static constexpr const char* td_1 = "res/levels/td_1.txt";

};