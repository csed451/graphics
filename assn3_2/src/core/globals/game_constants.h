#pragma once

#include <glm/glm.hpp>

constexpr float MAX_COORD = 50;
constexpr int ENEMY_MAX_HEART = 10;
constexpr int FPS = 60;

constexpr float TWO_PI  = 6.2831853f;
constexpr float DEG2RAD = 0.017453292f;

inline const glm::vec3 UP = glm::vec3(0, 1, 0);
inline const glm::vec3 DOWN = glm::vec3(0, -1, 0);
inline const glm::vec3 RIGHT = glm::vec3(1, 0, 0);
inline const glm::vec3 LEFT = glm::vec3(-1, 0, 0);
inline const glm::vec3 FORWARD = glm::vec3(0, 0, 1);
inline const glm::vec3 BACKWARD = glm::vec3(0, 0, -1);
inline const glm::vec3 ZERO = glm::vec3(0, 0, 0);


inline bool is_outside_window(glm::vec3 pos) {
    return pos.x > MAX_COORD || pos.x < -MAX_COORD || pos.y > MAX_COORD  || pos.y < -MAX_COORD || pos.z > MAX_COORD/4 || pos.z < -MAX_COORD;
}
