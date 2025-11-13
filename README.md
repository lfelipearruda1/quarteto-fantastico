gcc -Wall -Wextra -std=c99 \
  -Isrc \
  -I"$(brew --prefix raylib)/include" \
  src/main.c \
  src/player/player.c \
  src/ranking/ranking.c \
  src/logo/logo.c \
  src/select/select.c \
  -o game \
  -L"$(brew --prefix raylib)/lib" \
  -lraylib -lm \
  -framework Cocoa -framework IOKit -framework CoreVideo
./game
