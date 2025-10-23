#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/globals/game_constants.h"

inline glm::mat4 cameraMatrix = glm::mat4(1.0f);
inline glm::vec3 cameraPos = glm::vec3(0, 0, MAX_COORD);
inline glm::vec3 cameraTarget = ZERO;
inline glm::vec3 cameraUpDirection = UP;

inline void update_camera() {
    cameraMatrix = glm::lookAt(cameraPos, cameraTarget, cameraUpDirection);
}

inline void init_camera() {
    cameraMatrix = glm::mat4(1.0f);
    cameraPos = glm::vec3(0, 0, MAX_COORD);
    cameraTarget = ZERO;
    cameraUpDirection = UP;
    update_camera();
}
