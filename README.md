# Quarteto Fantástico — Builds & Execução (WSL / Linux / macOS)

<p align="center">
  <img src="https://img.shields.io/badge/Language-C-blue.svg" alt="Linguagem: C">
  <img src="https://img.shields.io/badge/Framework-Raylib-red.svg" alt="Framework: Raylib">
  <img src="https://img.shields.io/badge/Platform-Linux%20%7C%20WSL%20%7C%20macOS-yellow.svg" alt="Plataformas: Linux | WSL | macOS">
</p>

## 🚀 Demonstração (Demo)
Confira o jogo em ação!

| Idioma | Link |
|-------|------|
| 🇧🇷 **Português** | 🎬 [Gameplay no YouTube](https://youtu.be/p4kI9Ytr3s8) |

---

## 🛠️ Dependências (Dependencies)

Para compilar e executar o projeto, você precisará das seguintes dependências:

- **Raylib**
- **gcc / build-essential**
- **pkg-config** (Linux/WSL)
- **Homebrew** (macOS)
- **Bibliotecas X11 / OpenGL** (Linux/WSL)

---

## 📦 Instalação e Configuração

### 🐧 Linux / WSL (Ubuntu)

#### 1. Instalar ferramentas e dependências gráficas
```bash
sudo apt update
sudo apt install build-essential git cmake pkg-config \
    libgl1-mesa-dev libx11-dev libxi-dev libxrandr-dev libxinerama-dev libxcursor-dev
```

#### 2. Clonar e instalar a Raylib
```bash
git clone https://github.com/raysan5/raylib.git
cd raylib/src
make
sudo make install
```

---

### 🍎 macOS

#### 1. Instalar Homebrew
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

#### 2. Instalar Raylib
```bash
brew install raylib
```

---

## 🏗️ Compilar o Jogo (Building the Game)

### ✔️ Opção 1 — Usando Makefile (Recomendado)

#### 🐧 Linux / WSL
```bash
make -f Makefile.linux
```

#### 🍎 macOS
```bash
make -f Makefile.mac
```

---

### 🔧 Opção 2 — Usando CMake (Linux/WSL)
```bash
mkdir build
cd build
cmake ..
make
```

O executável será gerado como **quarteto** dentro da pasta **build/**.

---

## ▶️ Executar o Jogo

### 1. Navegue até o diretório raiz
É fundamental que o executável seja executado a partir da raiz do projeto para que a pasta `assets/` seja encontrada corretamente.

### 2. Executar o jogo

#### Usando Makefile
```bash
./game
```

#### Usando CMake
```bash
./build/quarteto
```

---

## ⚠️ Notas Importantes

- O jogo utiliza **Raylib + OpenGL**.
- No **WSL**, configure suporte gráfico (WSLg ou servidor X).
- A pasta **assets/** deve estar no mesmo diretório de execução.

