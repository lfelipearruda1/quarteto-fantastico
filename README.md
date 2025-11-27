# Quarteto Fantástico — Builds & Execução (WSL / Linux / macOS)

<p align="center">
  <img src="https://img.shields.io/badge/Language-C-blue.svg" alt="Linguagem: C">
  <img src="https://img.shields.io/badge/Framework-Raylib-red.svg" alt="Framework: Raylib">
  <img src="https://img.shields.io/badge/Platform-Linux%20%7C%20WSL%20%7C%20macOS-yellow.svg" alt="Plataformas: Linux | WSL | macOS">
</p>

## 🚀 Demonstração (Demo)
Confira o jogo em ação!

| Idioma | Link |
| :--- | :--- |
| **Português** | [Gameplay no YouTube](https://youtu.be/p4kI9Ytr3s8) 🎬 |

---

## 🛠️ Dependências (Dependencies)
Para compilar e executar o projeto, você precisará das seguintes dependências:

* **Raylib** (biblioteca de jogos simples e fácil de usar)
* **gcc / build-essential** (compilador C e ferramentas de build)
* **pkg-config** (Apenas para Linux/WSL)
* **Bibliotecas de X11 / OpenGL** (para renderização gráfica - Apenas para Linux/WSL)

---

## 📦 Instalação e Configuração

### 🐧 Linux / WSL (Ubuntu)
1. **Instalar Ferramentas e Dependências Gráficas:**
   ```bash
   sudo apt update
   sudo apt install build-essential git cmake pkg-config \
       libgl1-mesa-dev libx11-dev libxi-dev libxrandr-dev libxinerama-dev libxcursor-dev
Clonar e Instalar a Raylib:

Bash

git clone [https://github.com/raysan5/raylib.git](https://github.com/raysan5/raylib.git)
cd raylib/src
make
sudo make install
🍎 macOS
Instalar Homebrew (Se ainda não tiver).

Instalar Raylib via Homebrew:

Bash

brew install raylib
🏗️ Compilar o Jogo (Building the Game)
Opção A: Usando CMake (Linux/WSL - Recomendado)
Execute os seguintes comandos a partir da raiz do projeto:

Bash

mkdir build
cd build
cmake ..
make
Opção B: Usando GCC (Compilação Direta)
🐧 Linux / WSL
Bash

gcc src/*.c -o quarteto -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
🍎 macOS
Certifique-se de que o Homebrew e a Raylib estão instalados. Execute o comando abaixo a partir da raiz do projeto:

Bash

gcc -Wall -Wextra -std=c99 \
    -I$(brew --prefix raylib)/include \
    src/*.c \
    -L$(brew --prefix raylib)/lib \
    -lraylib -lm \
    -framework Cocoa -framework IOKit -framework CoreVideo \
    -o quarteto
Nota: Substitua src/*.c pela lista completa dos seus arquivos .c se a expansão de curinga não funcionar como esperado. O executável final será chamado de quarteto.

▶️ Executar o Jogo (Running the Game)
1. Navegue até o Diretório Certo
É crucial que o executável seja executado a partir do diretório raiz do repositório para que a pasta assets/ seja encontrada.

2. Executar
Se você usou CMake (Opção A):

Bash

./build/quarteto
Se você usou GCC (Opção B):

Bash

./quarteto
ou

Bash

./game # (Se você mudou o nome de saída para 'game' no macOS)
⚠️ Observações Importantes (Notes)
Gráficos: O jogo utiliza a Raylib e OpenGL para renderização.

WSL: Se estiver no WSL, você deve ter o suporte gráfico configurado (usando WSLg ou um servidor X configurado) para que a janela do jogo seja exibida.

Assets: A pasta assets/ (contendo texturas, sprites, etc.) deve estar no mesmo nível de diretório de onde o executável é chamado (o diretório de trabalho atual), não onde o executável está localizado.
