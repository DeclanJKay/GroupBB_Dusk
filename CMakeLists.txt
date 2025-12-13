cmake_minimum_required(VERSION 3.11)
project(tile_engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==== Output dirs (bin/Debug, bin/Release, etc.) ====
set(OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
foreach(cfg Debug Release RelWithDebInfo MinSizeRel)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${cfg} "${OUTPUT_DIRECTORY}/${cfg}")
endforeach()

# ==== SFML submodule ====
add_subdirectory("lib/SFML")
set(SFML_INCS "lib/SFML/include")
link_directories("${CMAKE_BINARY_DIR}/lib/SFML/lib")

add_subdirectory("lib/box2d")
set(BOX2D_INCS "lib/SFML/include")
link_directories("${CMAKE_BINARY_DIR}/lib/box2d/lib")


# ==== Level system library (build FIRST) ====
add_library(tile_level STATIC
  tile_level_loader/level_system.cpp)
target_include_directories(tile_level INTERFACE tile_level)
target_link_libraries(tile_level sfml-graphics)

# ==== Game executable ====
add_executable(tile_engine
  main.cpp
  gameSys.cpp
  MouseHelper.cpp
  KeyboardHelper.cpp
  Scenes.cpp
  EnemyStats.cpp
  FileMgr.cpp
  Weapons.cpp
  Turrets.cpp

  # headers optional (for VS visibility)
  Scenes.hpp
  MouseHelper.hpp
  KeyboardHelper.hpp
  Comps.hpp
  gameParams.hpp
  gameSys.hpp
  Reg.hpp
  Systems.hpp
  EnemyStats.hpp
  GenericHelpers.hpp
  FileMgr.hpp
  Weapons.hpp
  Turrets.hpp
    )

# Link SFML + our library
target_include_directories(tile_engine PRIVATE ${SFML_INCS} ${BOX2D_INCS} tile_level)
target_link_libraries(tile_engine sfml-graphics box2d tile_level)

# ==== Copy resources ====
add_custom_target(copy_resources ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${PROJECT_SOURCE_DIR}/res"
          "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${PROJECT_SOURCE_DIR}/res"
          "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/res"
  VERBATIM
)
add_dependencies(tile_engine copy_resources)

# ==== VS debugger working dir ====
set_target_properties(tile_engine PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
)