#pragma once

#include "core/base/object.h"

class Attack : public Object {
private:
    int damage = 1;
    glm::vec3 velocity = glm::vec3(0, 80, 0);
public:
    Attack(
        glm::vec3 _pos = ZERO, 
        GLfloat _angle = 0, 
        glm::vec3 _axis = UP, 
        glm::vec3 _size = glm::vec3(1), 
        glm::vec3 _center = ZERO
    ) : Object(_pos, _angle, _axis, _size, _center) {
        set_mesh(load_mesh("assets/models/rice.obj"));     
    };

    void draw_shape() const override;
    void update_logic(float deltaTime) override;

    int get_damage() { return damage; }
    void set_velocity(glm::vec3 v) { velocity = v; }
};
