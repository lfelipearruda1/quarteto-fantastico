gcc main.c -o game \
  -I"$(brew --prefix raylib)/include" \
  -L"$(brew --prefix raylib)/lib" \
  -lraylib -lm \
  -framework Cocoa -framework IOKit -framework CoreVideo

./game
