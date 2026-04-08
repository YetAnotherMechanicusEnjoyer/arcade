##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## Compiles CXX files
##

CC	= g++

SRC_DIR		=	src/
DIR_OBJ		=	.obj/
LIB_DIR		=	lib/
GAMES_DIR	=	games/
GRAPH_DIR	=	graphicals/


CORE_FILES	=	main.cpp	\
							Core.cpp
CORE				=	$(addprefix $(SRC_DIR), $(CORE_FILES))
CORE_OBJ		=	$(CORE:%.cpp=$(DIR_OBJ)%.o)
BINARY			=	arcade

SNAKE				=	$(SRC_DIR)$(GAMES_DIR)Snake/SnakeModule.cpp
SNAKE_OBJ		=	$(SNAKE:%.cpp=$(DIR_OBJ)%.o)
SNAKE_LIB 	=	$(LIB_DIR)arcade_snake.so

NCURSES				=	$(SRC_DIR)$(GRAPH_DIR)Ncurses/NcursesModule.cpp
NCURSES_OBJ		=	$(NCURSES:%.cpp=$(DIR_OBJ)%.o)
NCURSES_LIB		=	$(LIB_DIR)arcade_ncurses.so
NCURSES_FLAGS	=	-lncurses

SDL2					=	$(SRC_DIR)$(GRAPH_DIR)Sdl2/Sdl2Module.cpp
SDL2_OBJ			=	$(SDL2:%.cpp=$(DIR_OBJ)%.o)
SDL2_LIB			=	$(LIB_DIR)arcade_sdl2.so
SDL2_FLAGS		=	-lSDL2 -lSDL2_ttf

CPPFLAGS			= -std=c++20 -Iinclude -fPIC -g -Wall -Wextra -Werror
LDFLAGS_CORE	=	-ldl -rdynamic
LDFLAGS_LIB		=	-shared


all:	core games graphicals

core: 	$(BINARY)

games:	$(SNAKE_LIB)

graphicals:	$(NCURSES_LIB) $(SDL2_LIB)

$(BINARY):	$(CORE_OBJ)
		@$(CC) $^ -o $@ $(LDFLAGS_CORE)
		@echo -e "\x1b[32m[OK] Executable $@\x1b[0m"

$(SNAKE_LIB):	$(SNAKE_OBJ)
		@mkdir -p $(dir $(LIB_DIR))
		@$(CC) $^ -o $@ $(LDFLAGS_LIB)
		@echo -e "\x1b[32m[OK] Library $@\x1b[0m"

$(NCURSES_LIB):	$(NCURSES_OBJ)
		@mkdir -p $(dir $(LIB_DIR))
		@$(CC) $^ -o $@ $(LDFLAGS_LIB) $(NCURSES_FLAGS)
		@echo -e "\x1b[32m[OK] Library $@\x1b[0m"

$(SDL2_LIB):	$(SDL2_OBJ)
		@mkdir -p $(dir $(LIB_DIR))
		@$(CC) $^ -o $@ $(LDFLAGS_LIB) $(SDL2_FLAGS)
		@echo -e "\x1b[32m[OK] Library $@\x1b[0m"


$(DIR_OBJ)%.o:	%.cpp
		@mkdir -p $(dir $@)
		@$(CC) $(CPPFLAGS) -c $< -o $@
		@echo -e "\x1b[36m[Compiling] $<\x1b[0m"

clean:
		@rm -rf $(DIR_OBJ)
		@echo -e "\x1b[35m[Clean] Objects removed\x1b[0m"

fclean:	clean
		@rm -f $(BINARY)
		@rm -rf $(LIB_DIR)
		@echo -e "\x1b[35m[Fclean] Binary and Libs removed\x1b[0m"

re:		fclean all

.PHONY: all core games graphicals clean fclean re
