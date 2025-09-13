# CSED451: Computer Graphics (Fall 2025) 

This repository is for team assignments in the CSED451: Computer Graphics (Fall 2025) course.

## Team Information
- **Team Name**: Endgame_Duo
- **Members**:
  - 20200120 SeonHo CHA
  - 20200703 SoonHo KIM

## Repository Structure
The repository is organized into the following subdirectories:
```bash
graphics
   ├── include/
   ├── assn1/
   │   ├── src/         # assn1 source code
   │   ├── docs/        # assn1 documents
   │   ├── build/       # assn1 outputs
   │   └── README.md    # assn1 description
   ├── assn2/
   ├── assn3/
   ├── assn4/
   └── README.md
```

Each subdirectory will contain the respective assignment and its detailed README.

</br>

## OpenGL Project Setup Guide

### Environment
- OS: Windows 11 with WSL2 (Ubuntu 22.04 LTS)
- Editor: Visual Studio Code 
- Compiler: g++

### Prerequisites
First, install the necessary libraries for OpenGL development on your WSL terminal.

```bash
sudo apt update
sudo apt install build-essential g++ libglew-dev libglm-dev freeglut3-dev
```

### Build & Run
Compile the source code and run the executable from the project root directory.
```bash
# Command Guide
g++ [source file path] -o [executable file name] -I ./include -lGL -lGLEW -lglut
```

As an example, let's compile the testbed.cpp in repo.

```bash
# Compile
g++ testbed.cpp -o testbed_wsl -I ./include -lGL -lGLEW -lglut

# Run
./testbed_wsl
```

If you see a blue triangle, you are all set!