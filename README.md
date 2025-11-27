# Quarteto Fantástico – Raylib (WSL2)

Jogo de plataforma desenvolvido em C + Raylib, rodando no WSL2 (Ubuntu).

---

## Instalação rápida (WSL2)

### 1. Dependências básicas

```bash
sudo apt update
sudo apt install -y build-essential git libx11-dev libgl1-mesa-dev libasound2-dev
2. Instalar a Raylib
bash
Copiar código
git clone https://github.com/raysan5/raylib
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install
Compilar o jogo
bash
Copiar código
make
Limpar:

bash
Copiar código
make clean
Executar
bash
Copiar código
./game
Windows 10 (VcXsrv)
Se estiver usando Windows 10, abra o VcXsrv com as opções:

Multiple windows

Native OpenGL desmarcado

Disable access control marcado

No Windows 11 não é necessário (WSLg já funciona).

Estrutura do projeto
css
Copiar código
src/
assets/
Makefile
players.txt
game (gerado após compilar)
yaml
Copiar código

---
