#pragma once
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

constexpr float ORTHO_LEFT = -50;
constexpr float ORTHO_RIGHT = 50;
constexpr float ORTHO_TOP = 50;
constexpr float ORTHO_BOTTOM = -50;

constexpr int FPS = 60;

constexpr float TWO_PI  = 6.2831853f;
constexpr float DEG2RAD = 0.017453292f;

inline const glm::vec3 UP = glm::vec3(0, 1, 0);
inline const glm::vec3 DOWN = glm::vec3(0, -1, 0);
inline const glm::vec3 RIGHT = glm::vec3(1, 0, 0);
inline const glm::vec3 LEFT = glm::vec3(-1, 0, 0);
inline const glm::vec3 ZERO = glm::vec3(0, 0, 0);

inline glm::mat4 CAMERA_MATRIX = glm::mat4(1);
inline glm::vec3 CAMERA_POS = glm::vec3(0, 0, 50);
inline glm::vec3 CAMERA_TARGET = glm::vec3(0, 0, 0);
inline glm::vec3 CAMERA_UP_DIRECTION = glm::vec3(0, 1, 0);


inline bool is_outside_window(glm::vec3 pos) {
    return pos.x > ORTHO_RIGHT || pos.x < ORTHO_LEFT|| pos.y > ORTHO_TOP || pos.y < ORTHO_BOTTOM;
}
inline void update_camera() {
    CAMERA_MATRIX = glm::lookAt(CAMERA_POS, CAMERA_TARGET, CAMERA_UP_DIRECTION);
}