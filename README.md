# Quarteto Fantástico — Builds & Execução (WSL / Linux / MACOS)

<p align="center">
  <img src="https://img.shields.io/badge/Language-C-blue.svg" alt="Linguagem: C">
  <img src="https://img.shields.io/badge/Framework-Raylib-red.svg" alt="Framework: Raylib">
  <img src="https://img.shields.io/badge/Platform-Linux%20%7C%20WSL-yellow.svg" alt="Plataformas: Linux | WSL">
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
* **pkg-config**
* **Bibliotecas de X11 / OpenGL** (para renderização gráfica):
    * `libgl1-mesa-dev`
    * `libx11-dev`
    * `libxi-dev`
    * `libxrandr-dev`
    * `libxinerama-dev`
    * `libxcursor-dev`

---

## 📦 Instalação da Raylib (Ubuntu / WSL)
Siga estes passos para instalar as dependências e a Raylib em sistemas baseados em **apt (Ubuntu, WSL)**.

### 1. Instalar Ferramentas de Build e Dependências Gráficas
```bash
sudo apt update
sudo apt install build-essential git cmake pkg-config \
    libgl1-mesa-dev libx11-dev libxi-dev libxrandr-dev libxinerama-dev libxcursor-dev
2. Clonar e Instalar a Raylib
Bash

git clone [https://github.com/raysan5/raylib.git](https://github.com/raysan5/raylib.git)
cd raylib/src
make
sudo make install
🏗️ Compilar o Jogo (Building the Game)
Você pode usar CMake ou compilar diretamente com GCC.

Opção A: Usando CMake (Recomendado)
Execute os seguintes comandos a partir da raiz do projeto:

Bash

mkdir build
cd build
cmake ..
make
Opção B: Usando GCC (Compilação Direta)
Execute este comando a partir da raiz do projeto ou ajuste os caminhos conforme necessário:

Bash

gcc src/*.c -o quarteto -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
Nota: Se você não usou a estrutura de diretórios build, ajuste o caminho de saída (-o quarteto) para o local desejado.

▶️ Executar o Jogo (Running the Game)
1. Navegue até o Diretório Certo
É crucial que o executável seja executado a partir do diretório raiz do repositório para que a pasta assets/ seja encontrada.

Bash

cd /caminho/para/seu/repositorio 
2. Executar
Se você usou CMake (Opção A):

Bash

./build/quarteto
Se você usou GCC (Opção B) e colocou o executável na raiz:

Bash

./quarteto
⚠️ Observações Importantes (Notes)
Gráficos: O jogo utiliza a Raylib e OpenGL para renderização.

WSL: Se estiver no WSL, você deve ter o suporte gráfico configurado (usando WSLg ou um servidor X configurado) para que a janela do jogo seja exibida.

Assets: A pasta assets/ (contendo texturas, sprites, etc.) deve estar no mesmo nível de diretório de onde o executável é chamado (o diretório de trabalho atual), não onde o executável está localizado.
