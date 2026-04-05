cmake_minimum_required(VERSION 3.16)
project(Arcade VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Werror)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(arcade ${SOURCES})

target_link_libraries(arcade PRIVATE ${CMAKE_DL_LIBS})

macro(add-arcade-library target_name src_files)
  add_library(${target_name} SHARED ${src_files})
  set_target_properties(${target_name} PROPERTIES PREFIX "")
endmacro()

#add-arcade-library(arcade_ncurses "src/graphicals/Ncurses/NcursesModule.cpp")
#target_link_libraries(arcade_ncurses ncurses)

#add-arcade-library(arcade_sdl2 "src/graphicals/Ncurses/Sdl2/Sdl2Module.cpp")
#target_link_libraries(arcade_sdl2 SDL2)

#add-arcade-library(arcade_sfml "src/graphicals/Sfml/SfmlModule.cpp")
#target_link_libraries(arcade_sfml sfml-graphics sfml-window sfml-system)

#add-arcade-library(arcade_snake "src/games/Snake/SnakeModule.cpp")
#add-arcade-library(arcade_pacman "src/games/Pacman/PacmanModule.cpp")
