#pragma once

#include "core/base/object.h"

class Enemy; 

class Healthbar : public Object {
private:
    glm::vec3 initialPos;
    GLfloat initialAngle;
    glm::vec3 initialAxis;
    glm::vec3 initialSize;
    glm::vec3 initialCenter;
    Enemy* parent;

public:
    Healthbar(
            glm::vec3 _pos = ZERO,
            GLfloat _angle = 0,
            glm::vec3 _axis = UP,
            glm::vec3 _size = glm::vec3(1),
            glm::vec3 _center = ZERO,
            Object* _parent = nullptr
        );

    void draw_shape() const override;

    void deactivate();
    void reset();
};
