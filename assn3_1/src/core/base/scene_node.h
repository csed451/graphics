#pragma once

#include "core/base/object.h"

class SceneNode : public Object {
public:
    SceneNode(glm::vec3 _pos = ZERO,
              GLfloat _angle = 0.0f,
              glm::vec3 _axis = UP,
              glm::vec3 _size = glm::vec3(1.0f),
              glm::vec3 _center = ZERO)
        : Object(_pos, _angle, _axis, _size, _center) {}

    void draw_shape() const override {}
};

inline SceneNode sceneRoot;