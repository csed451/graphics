## Assn2 Overview
Simple 2D shooting game enhanced with hierarchical animation.

## End-User Guide

### 1. Compilation
Move to the src directory (graphics/assn2/src) and run one of the provided g++ commands.  
If successful, an executable named `main` is generated in the same directory.
```bash
# on graphics/assn2/src
cd ./assn2/src

# Using WildCard
g++ main.cpp base/*.cpp entities/*.cpp -o main \
    -I. -Ibase -Ientities -I../../include \
    -lGL -lGLEW -lglut
```

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