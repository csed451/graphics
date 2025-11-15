
# Assn3-2 Overview

Assignment 3-2 extends the bullet hell prototype with multiple rendering styles and camera perspectives. The game cycles between wireframe and opaque rendering with `W`, and `C` rotates the camera through top-perspective, top-orthographic, and close third-person views.

## End-User Guide

### 1. Build & Run

Move to the src directory (`graphics/assn3_2/src`) and run one of the provided Makefile commands.

From `./assn3_2/src/`:

- `make` (or `make all`): build the `main` executable into `assn3_1/src/main`.
- `make run`: build (if needed) and launch the game.
- `make clean`: remove `build/` artifacts and `main`.

<details>
    <summary> If you want, you can compile and execute directly with the command below. </summary>

```bash
# on graphics/assn3_2/src
cd ./assn3_2/src

g++ app/main.cpp \
    core/base/object.cpp \
    core/render/mesh.cpp \
    game/entities/player.cpp \
    game/entities/enemy.cpp \
    game/weapons/attack.cpp \
    game/weapons/canon.cpp \
    game/weapons/bullet.cpp \
    game/attachments/upper.cpp \
    game/attachments/escort_plane.cpp \
    game/attachments/lower.cpp \
    game/attachments/hand.cpp \
    game/ui/healthbar.cpp \
    -o main \
    -I. -I../../include \
    -lGL -lGLEW -lglut

./main
```

</details>

---

### 2. Controls

Movement: **Arrow keys**

Combat: **Space (hold)** >>  fire

Render Style: **W / w**

- cycles : opaque → wireframe → ...

Camera Views: **C / c**

- cycles : top-perspective → top-orthographic → third-person → ...

Game Over:

- **R / r**: restart
- **Q / q / ESC**: quit

---

### 3 Notes

Ensure OpenGL, GLEW, and GLUT development packages are installed.
For additional details, refer to the [main project README](../README.md).
