

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
* **Homebrew** (Apenas para macOS)
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
O método recomendado é usar o Makefile apropriado para o seu sistema.

Opção 1: Usando Makefile (Recomendado)
🐧 Linux / WSL
Se o seu sistema for Linux (ou WSL), use o Makefile configurado para as bibliotecas padrão do sistema.

Salve o conteúdo abaixo como Makefile.linux na raiz do projeto.

Na raiz do projeto, execute:

Bash

make -f Makefile.linux
🍎 macOS
Se o seu sistema for macOS, use o Makefile que utiliza caminhos do Homebrew para a Raylib.

Salve o conteúdo abaixo como Makefile.mac na raiz do projeto.

Na raiz do projeto, execute:

Bash

make -f Makefile.mac
Opção 2: Usando CMake (Linux/WSL)
Como alternativa no Linux/WSL, você ainda pode usar o CMake (se preferir uma ferramenta de build de nível superior):

Bash

mkdir build
cd build
cmake ..
make
▶️ Executar o Jogo (Running the Game)
1. Navegue até o Diretório Certo
É fundamental que o executável seja executado a partir do diretório raiz do repositório para que a pasta assets/ seja encontrada corretamente.

2. Executar
O executável gerado se chamará game (usando os Makefiles) ou quarteto (usando CMake na pasta build).

Se usou o Makefile (game):

Bash

./game
Se usou CMake (quarteto):

Bash

./build/quarteto
⚠️ Observações Importantes (Notes)
Gráficos: O jogo utiliza a Raylib e OpenGL para renderização.

WSL: Se estiver no WSL, você deve ter o suporte gráfico configurado (usando WSLg ou um servidor X configurado) para que a janela do jogo seja exibida.

Assets: A pasta assets/ (contendo texturas, sprites, etc.) deve estar no mesmo nível de diretório de onde o executável é chamado (o diretório de trabalho atual).
