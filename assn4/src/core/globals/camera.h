#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/globals/game_constants.h"
#include "core/base/object.h"
#include "core/render/renderer.h"

inline glm::mat4 cameraMatrix = glm::mat4(1.0f);
inline glm::vec3 cameraPos = glm::vec3(0, 0, MAX_COORD);
inline glm::vec3 cameraTarget = ZERO;
inline glm::vec3 cameraUpDirection = UP;
inline Object* cameraTargetObject = nullptr;
inline glm::mat4 projectionMatrix = glm::mat4(1.0f);

enum class ProjectionType { Perspective, Orthographic, Thirdperson };
inline ProjectionType projectionType = ProjectionType::Perspective;

inline void update_camera() {
    if (projectionType == ProjectionType::Orthographic)
        cameraMatrix = glm::mat4(1.0f);
    else
        if (cameraTargetObject)
            cameraMatrix = glm::lookAt(cameraTargetObject->get_pos() + cameraPos, cameraTargetObject->get_pos(), cameraUpDirection);
        else
            cameraMatrix = glm::lookAt(cameraPos, cameraTarget, cameraUpDirection);
    gRenderer.set_view(cameraMatrix);
}

inline void init_camera(glm::vec3 p=glm::vec3(0, 0, MAX_COORD), glm::vec3 t= ZERO, glm::vec3 u=UP) {
    cameraPos = p;
    cameraTarget = t;
    cameraUpDirection = u;
    update_camera();
}
