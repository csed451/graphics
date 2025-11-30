#pragma once

#include <string>
#include <GL/glew.h>

struct Texture2D {
    GLuint id = 0;
    int width = 0;
    int height = 0;
};

// Load an 8-bit per channel PNG into an OpenGL 2D texture.
// Returns {0,0,0} on failure.
Texture2D load_texture_png(const std::string& path,
                           bool generateMips = true,
                           bool flipY = true);
