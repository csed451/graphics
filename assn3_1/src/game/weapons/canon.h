#pragma once

#include "core/base/object.h"
#include "core/base/object_pool.h"
#include "attack.h"


class Canon : public Object {
private:
    ObjectPool<Attack> attackPool;
    float shootInterval = 0.2;
    float shootCooldown = shootInterval;
public:
    Canon(
        glm::vec3 _pos = ZERO, 
        GLfloat _angle = 0, 
        glm::vec3 _axis = UP, 
        glm::vec3 _size = glm::vec3(1), 
        glm::vec3 _center = glm::vec3(0, -0.5, 0)
    ) : Object(_pos, _angle, _axis, _size, _center), attackPool(50) {
        set_mesh(load_mesh("assets/square.obj"));     
    };

    void draw_shape() const override;
    void update(float deltaTime);

    ObjectPool<Attack>& get_attackPool() { return attackPool; }

    void shoot();
};