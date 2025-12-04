# Assn4 Overview

Assignment 4 extends the bullet hell prototype with multiple rendering styles, camera perspectives, and textured environments.

## End-User Guide

### 0. Additional Setting
You have to install one additional library for assn4.

```bash
sudo apt-get install libpng-dev
```

### 1. Build & Run

Move to the `src` directory (`graphics/assn4/src`) and run one of the provided Makefile commands.

From `./assn4/src/`:

- `make` (or `make all`): build the `main` executable into `assn4/src/main`.
- `make run`: build (if needed) and launch the game.
- `make clean`: remove `build/` artifacts and `main`.

<details>
  <summary>If you want, you can compile and execute directly with the command below.</summary>

```bash
# on graphics/assn4/src
cd ./assn4/src

g++ app/main.cpp \
    core/base/object.cpp \
    core/render/shader_program.cpp \
    core/render/renderer.cpp \
    core/render/mesh.cpp \
    game/entities/player.cpp \
    game/entities/enemy.cpp \
    game/weapons/attack.cpp \
    game/weapons/canon.cpp \
    game/weapons/bullet.cpp \
    game/attachments/upper.cpp \
    game/attachments/lower.cpp \
    game/attachments/escort_plane.cpp \
    game/ui/healthbar.cpp \
    -I. -I../../include \
    -std=c++17 \
    -o main \
    -lGL -lGLEW -lglut

./main
```
</details>

---

### 2. Controls

Movement: **Arrow keys**

Combat: **Space (hold)** — fire

Shading Mode: **W / w**

- cycles: Gouraud → Phong → Phong + Normal Map (changes shading only)

Camera Views: **C / c**

- cycles: top-perspective ↔ close third-person (no orthographic mode)

Pause/Unpause updates: **T / t** (pauses/resumes game updates and animations)

Day/Night toggle (environment textures): **B / b**

- switches between day and night sky/ocean textures (background only)

Shadow toggle: **S / s**

- generate shadows made by directional light (Phong & Phong Normal only)

Motion blur toggle: **M / m**

- apply a motion blur effect to objects in motion.

Game Over:

- **R / r**: restart
- **Q / q / ESC**: quit

---

### 3 Notes

Ensure OpenGL, GLEW, and GLUT development packages are installed.
For additional details, refer to the [main project README](../README.md).
The render-style toggle (wireframe / hidden-line) remains implemented, but key bindings are intentionally left empty.
