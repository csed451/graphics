#pragma once
#include <glm/glm.hpp>
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

inline bool is_outside_window(glm::vec3 pos) {
    return pos.x > ORTHO_RIGHT || pos.x < ORTHO_LEFT|| pos.y > ORTHO_TOP || pos.y < ORTHO_BOTTOM;
}