#pragma once

#include "object.h"

class Bullet : public Object {
private:
    glm::vec3 direction = glm::vec3(0, -1, 0);
    float velocity = 20.0f;
public:
    Bullet(glm::vec3 _pos=glm::vec3(), 
           GLfloat _angle=0, 
           glm::vec3 _axis=glm::vec3(1,0,0), 
           glm::vec3 _size=glm::vec3(1), 
           glm::vec3 _center=glm::vec3()) 
        : Object(_pos, _angle, _axis, _size, _center) {};

    void draw_shape() const override;
    void update(float deltaTime);

    void set_direction(glm::vec3 dir) { direction = dir;}
};