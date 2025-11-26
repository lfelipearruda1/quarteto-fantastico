CC = gcc
CFLAGS = -Wall -Wextra -std=c99
RAYLIB_INCLUDE = $(shell brew --prefix raylib)/include
RAYLIB_LIB = $(shell brew --prefix raylib)/lib
SRC_DIR = src
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/player/player.c \
          $(SRC_DIR)/ranking/ranking.c \
          $(SRC_DIR)/logo/logo.c \
          $(SRC_DIR)/select/select.c \
          $(SRC_DIR)/game/game.c \
          $(SRC_DIR)/game/levels/level-common.c \
          $(SRC_DIR)/game/levels/level-tocha.c \
          $(SRC_DIR)/game/levels/level-homem-elastico.c \
          $(SRC_DIR)/game/levels/level-mulher-invisivel.c \
          $(SRC_DIR)/game/levels/level-coisa.c
TARGET = game

INCLUDES = -I$(SRC_DIR) -I$(RAYLIB_INCLUDE)
LIBS = -L$(RAYLIB_LIB) -lraylib -lm -framework Cocoa -framework IOKit -framework CoreVideo

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: clean

