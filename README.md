🕹️ Quarteto Fantástico – Plataforma Raylib (WSL2/Linux)

Este projeto é um jogo de plataforma desenvolvido com C + Raylib, adaptado para rodar no WSL2 (Ubuntu) usando renderização via X11.

✅ Requisitos

Antes de compilar, é necessário ter:

1. Dependências do sistema

Instale de uma vez:

sudo apt update
sudo apt install -y build-essential cmake git gcc g++ \
    libx11-dev libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev \
    libgl1-mesa-dev libglu1-mesa-dev libasound2-dev

2. Instalar a Raylib localmente

Baixar e compilar:

git clone https://github.com/raysan5/raylib
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install


Isso instala:

/usr/local/include/raylib.h

/usr/local/lib/libraylib.a

3. Ter um servidor gráfico ativo no Windows

Para rodar janelas gráficas no WSL2 você precisa de:

WSLg (já vem no Windows 11)
ou

XServer externo (VcXsrv / Xming) se estiver no Windows 10

Se usar Windows 11 ou WSLg: não precisa fazer nada.

🔧 Como compilar o jogo

Dentro da pasta quarteto-raylib, execute:

make clean
make


Se tudo correr bem, o arquivo binário game será gerado.

▶️ Como executar

Ainda na pasta quarteto-raylib:

./game


O jogo deve abrir numa janela gráfica normal pelo WSL2.

🛠️ Estrutura do projeto
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
make clean

❗ Erros comuns
1. undefined reference to InitWindow, DrawText, etc

A raylib não está sendo linkada corretamente.
Verifique se o Makefile contém:

-L/usr/local/lib -lraylib -lm -lpthread -ldl -lrt -lX11

2. error: X11/Xlib.h: No such file or directory

Instale:

sudo apt install libx11-dev

3. Janela não abre

Se estiver no Windows 10, abra o VcXsrv antes:

Modo: Multiple windows

Desabilitar Native OpenGL

Habilitar Disable access control
