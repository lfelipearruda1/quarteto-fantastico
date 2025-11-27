CC = gcc
CFLAGS = -Wall -Wextra -std=c99
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
INCLUDES = -I$(SRC_DIR)

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)   # macOS
	RAYLIB_PREFIX := $(shell brew --prefix raylib)
	INCLUDES += -I$(RAYLIB_PREFIX)/include
	LIBS = -L$(RAYLIB_PREFIX)/lib -lraylib -lm \
	       -framework Cocoa -framework IOKit -framework CoreVideo
else                       # Linux
	INCLUDES +=
	LIBS = -lraylib -lm -lpthread -ldl -lrt -lX11
endif

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: clean
