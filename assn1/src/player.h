#pragma once

#include "object.h"
#include "objectPool.h"
#include "attack.h"

class Player : public Object {
private:
    ObjectPool<Attack> attackPool;
public:
    Player(glm::vec3 _pos=glm::vec3(), GLfloat _angle=0, glm::vec3 _axis=glm::vec3(1,0,0), glm::vec3 _size=glm::vec3(1), glm::vec3 _center=glm::vec3()) : Object(_pos, _angle, _axis, _size, _center), attackPool(5) {};

    void draw_shape() const override;

    ObjectPool<Attack>& get_attackPool() { return attackPool; }

    void shoot();
};