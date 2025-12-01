#pragma once

#include <GL/glew.h>

namespace background {

// Initialize floor geometry/texture; call once after renderer init.
// maxCoord is the gameplay MAX_COORD to scale the floor size.
void init(float maxCoord, bool dayMode);

// Draw the floor. Assumes renderer frame has begun.
void draw();

// Optional: change day/night textures and reload if needed.
void set_day_mode(bool dayMode);
bool is_day_mode();

// Release GL resources (optional).
void shutdown();

} // namespace background
