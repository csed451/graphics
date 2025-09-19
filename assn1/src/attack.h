#pragma once

#include "object.h"

class Attack : public Object {
private:
    int damage=1;
    glm::vec3 velocity = glm::vec3(0, 20, 0);
public:
    Attack(glm::vec3 _pos=glm::vec3(), GLfloat _angle=0, glm::vec3 _axis=glm::vec3(1,0,0), glm::vec3 _size=glm::vec3(1), glm::vec3 _center=glm::vec3()) : Object(_pos, _angle, _axis, _size, _center) {};

    void draw_shape() const override;
    void update(float deltaTime);
};