## Assn2 Overview
Simple 2D shooting game enhanced with hierarchical animation.

## End-User Guide

### 1. Build & Run
Move to the src directory (graphics/assn2/src) and run one of the provided Makefile Command.

From `./assn2/src/`:
- `make` (or `make all`): build the `main` executable into `assn2/src/main`.
- `make run`: build (if needed) and launch the game.
- `make clean`: remove `build/` artifacts and `main`.

<details>
<summary>If you want, you can compile and execute directly with the command below.</summary>

```bash
# on graphics/assn2/src
cd ./assn2/src

g++ app/main.cpp \
    core/base/object.cpp \
    game/entities/player.cpp \
    game/entities/enemy.cpp \
    game/weapons/attack.cpp \
    game/weapons/canon.cpp \
    game/weapons/bullet.cpp \
    game/attachments/upper.cpp \
    game/attachments/lower.cpp \
    game/attachments/hand.cpp \
    game/ui/healthbar.cpp \
    -o main \
    -I. -I../../include \
    -lGL -lGLEW -lglut

./main
```

</details>

<br>

### 2. Controls
Movement:
- Arrow keys

Combat:
- Space (hold): fire

Game Over:
- R / r: restart
- Q / q / ESC: quit


<br>

### 3 Notes
Ensure OpenGL, GLEW, and GLUT development packages are installed.
For additional details, refer to the [main project README](../README.md).