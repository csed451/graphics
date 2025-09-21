## Overview
Simple 2D shooting game using OpenGL (GL, GLEW, GLUT). 



## Compile Command
```bash
# on graphics/assn1/src
g++ main.cpp base/object.cpp entities/player.cpp entities/canon.cpp entities/attack.cpp entities/enemy.cpp entities/bullet.cpp \
    -o main -I. -Ibase -Ientities -I../../include -lGL -lGLEW -lglut

# Using WildCard
g++ main.cpp base/*.cpp entities/*.cpp -o main \
    -I. -Ibase -Ientities -I../../include \
    -lGL -lGLEW -lglut
```

- ``-I`` (Uppercase): Include Path

- ``-L`` (Uppercase): Library Path

- ``-l`` (Lowercase): Link Library

## End-User Guide

### 1. Compilation
Move to the src directory (graphics/assn1/src) and run one of the provided g++ commands.  
If successful, an executable named `main` is generated in the same directory.

### 2. Execution
Run:
```bash
./main
```

### 3. Controls
Movement:
- Arrow Up: Move up
- Arrow Down: Move down
- Arrow Left: Move left
- Arrow Right: Move right

Attack:
- Space (hold): Continuous fire

Game Over:
- R / r: Restart
- Q / q / ESC: Quit

### 4. Notes
Ensure OpenGL, GLEW, and GLUT development packages are installed.
For additional details, refer to the [main project README](../README.md).