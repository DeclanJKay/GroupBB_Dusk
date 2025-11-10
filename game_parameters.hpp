//game_parameters.hpp
#pragma once //insure that this header file is included only once and there will no multiple definition of the same thing

struct Parameters {
    static constexpr unsigned int game_width = 800;
    static constexpr unsigned int game_height = 600;
    static constexpr float time_step = 1.0f / 60.0f; // add this
    // inside Parameters (struct or namespace)
    static constexpr const char* maze_1 = "res/levels/maze_1.txt";
    static constexpr const char* maze_2 = "res/levels/maze_2.txt";

};