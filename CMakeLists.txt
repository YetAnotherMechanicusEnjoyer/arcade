cmake_minimum_required(VERSION 3.16)
project(Arcade VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Werror)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

set(GAME_DIR "src/games/")
set(GRAPHICALS_DIR "src/graphicals/")

file(GLOB_RECURSE SOURCES "src/*.cpp")

foreach(TMP_PATH ${SOURCES})
  string(FIND ${TMP_PATH} ${GAME_DIR} GAME_DIR_FOUND)
  string(FIND ${TMP_PATH} ${GRAPHICALS_DIR} GRAPHICALS_DIR_FOUND)

  if(NOT ${GAME_DIR_FOUND} EQUAL -1 OR NOT ${GRAPHICALS_DIR_FOUND} EQUAL -1)
    list(REMOVE_ITEM SOURCES ${TMP_PATH})
  endif()
endforeach()

add_executable(arcade ${SOURCES})

target_link_libraries(arcade PRIVATE ${CMAKE_DL_LIBS})

macro(add_arcade_library target_name src_files)
  add_library(${target_name} SHARED ${src_files})
  set_target_properties(${target_name} PROPERTIES PREFIX "")
endmacro()

add_arcade_library(arcade_ncurses "src/graphicals/Ncurses/NcursesModule.cpp")
target_link_libraries(arcade_ncurses ncurses)

#add_arcade_library(arcade_sdl2 "src/graphicals/Ncurses/Sdl2/Sdl2Module.cpp")
#target_link_libraries(arcade_sdl2 SDL2)

#add_arcade_library(arcade_sfml "src/graphicals/Sfml/SfmlModule.cpp")
#target_link_libraries(arcade_sfml sfml-graphics sfml-window sfml-system)

add_arcade_library(arcade_snake "src/games/Snake/SnakeModule.cpp")
#add_arcade_library(arcade_pacman "src/games/Pacman/PacmanModule.cpp")
