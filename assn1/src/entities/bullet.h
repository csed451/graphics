#pragma once

#include "object.h"

class Bullet : public Object {
private:
    glm::vec3 direction = DOWN;
    mutable bool counter = false; 
    float velocity = 20.0f;
public:
    Bullet(
        glm::vec3 _pos=glm::vec3(), 
        GLfloat _angle=0, 
        glm::vec3 _axis=RIGHT, 
        glm::vec3 _size=glm::vec3(1), 
        glm::vec3 _center=ZERO
    ) : Object(_pos, _angle, _axis, _size, _center) {};

    void draw_shape() const override;
    void update(float deltaTime);

    void set_direction(glm::vec3 dir) { direction = dir; }
    void set_counter(bool c) { counter = c; }
};