# Program name
NAME = WebServ

# compiler
CC = c++

# compiler flags
CFLAGS =  -std=c++98 -Wall -Werror -Wextra -MMD #-fsanitize=address
DEBUG_FLAGS = -g

# src files and path for them
VPATH := $(shell find src -type d)
INCS := $(wildcard $(addsuffix /*.hpp, $(VPATH)))
INCLUDE = $(addprefix -I, $(VPATH))

# src files
SRC := main.cpp WebServerProg.cpp api_helpers.cpp api_get.cpp parser.cpp validateServers.cpp utils.cpp cgiHandler.cpp

# obj files and path for them
OBJ_DIR = ./obj
OBJ =$(addprefix $(OBJ_DIR)/, $(SRC:%.cpp=%.o))

#dependencies
DEP =$(OBJ:.o=.d)

# rules
all: $(NAME)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: all

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(NAME): $(OBJ_DIR) $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJ) -o $@
	
-include $(DEP)

$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all 

.PHONY: all debug clean fclean re