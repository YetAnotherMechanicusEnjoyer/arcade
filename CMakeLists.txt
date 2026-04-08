cmake_minimum_required(VERSION 3.16)
project(Arcade VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-g -Wall -Wextra -Werror)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

file(GLOB_RECURSE ALL_SOURCES "src/*.cpp")
set(CORE_SOURCES "")

foreach(FILE ${ALL_SOURCES})
  if(NOT FILE MATCHES "/games/" AND NOT FILE MATCHES "/graphicals/")
    list(APPEND CORE_SOURCES ${FILE})
  endif()
endforeach()

add_executable(arcade ${CORE_SOURCES})
target_link_libraries(arcade PRIVATE ${CMAKE_DL_LIBS})

macro(add_arcade_library target_name)
  add_library(${target_name} SHARED ${ARGN})
  set_target_properties(${target_name} PROPERTIES PREFIX "")
endmacro()

add_arcade_library(arcade_ncurses "src/graphicals/Ncurses/NcursesModule.cpp")
target_link_libraries(arcade_ncurses PRIVATE ncurses)

add_arcade_library(arcade_sdl2 "src/graphicals/Sdl2/Sdl2Module.cpp")
target_link_libraries(arcade_sdl2 PRIVATE SDL2 SDL2_ttf)

#add_arcade_library(arcade_sfml "src/graphicals/Sfml/SfmlModule.cpp")
#target_link_libraries(arcade_sfml sfml-graphics sfml-window sfml-system)

add_arcade_library(arcade_snake "src/games/Snake/SnakeModule.cpp")
#add_arcade_library(arcade_pacman "src/games/Pacman/PacmanModule.cpp")
