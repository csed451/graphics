#include "globals.h"

float orthoLeft = -50;
float orthoRight = 50;
float orthoTop = 50;
float orthoBottom = -50;

bool is_outside_window(glm::vec3 pos) {
    return pos.x > orthoRight || pos.x < orthoLeft || pos.y > orthoTop || pos.y < orthoBottom;
}