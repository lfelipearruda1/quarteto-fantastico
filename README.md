# 🕹️ Quarteto Fantástico – Plataforma Raylib (WSL2/Linux)

Este projeto é um jogo de plataforma desenvolvido com **C + Raylib**, adaptado para rodar no **WSL2 (Ubuntu)** usando renderização via **X11**.

## ✅ Requisitos

### 1. Dependências do sistema

Instale tudo de uma vez:

```bash
sudo apt update
sudo apt install -y build-essential cmake git gcc g++ \
    libx11-dev libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev \
    libgl1-mesa-dev libglu1-mesa-dev libasound2-dev
2. Instalar a Raylib localmente
Baixe e compile:

bash
Copiar código
git clone https://github.com/raysan5/raylib
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install
Arquivos instalados (exemplo):

swift
Copiar código
/usr/local/include/raylib.h
/usr/local/lib/libraylib.a
3. Servidor gráfico no Windows (WSL2)
Windows 11 (WSLg): já vem configurado — nada a fazer.

Windows 10: instale/use um XServer (VcXsrv / Xming) e inicie antes de rodar o jogo.

🔧 Como compilar o jogo
Dentro da pasta quarteto-raylib:

bash
Copiar código
make clean
make
Se tudo der certo, o binário game será gerado.

▶️ Como executar
Ainda na pasta do projeto:

bash
Copiar código
./game
O jogo abrirá em janela gráfica via WSL2.

🛠️ Estrutura do projeto
css
Copiar código
quarteto-raylib/
│
├─ src/
│  ├─ main.c
│  ├─ player/
│  ├─ game/
│  ├─ ranking/
│  ├─ logo/
│  └─ select/
│
├─ assets/
├─ players.txt
└─ Makefile
🧹 Limpar binários
bash
Copiar código
make clean
❗ Erros comuns & soluções
1) undefined reference to InitWindow, DrawText, etc.
Raylib não está sendo linkada. Verifique o Makefile para conter a linha de link:

bash
Copiar código
-L/usr/local/lib -lraylib -lm -lpthread -ldl -lrt -lX11
2) error: X11/Xlib.h: No such file or directory
Instale a dependência:

bash
Copiar código
sudo apt install libx11-dev
3) Janela não abre no WSL2 (Windows 10 + VcXsrv)
Abra o VcXsrv com:

Mode: Multiple windows

Desmarcar Native OpenGL

Marcar Disable access control

Inicie o VcXsrv antes de rodar ./game.
