#pragma once

#include "core/base/object.h"

class Attack : public Object {
private:
    int damage = 1;
    glm::vec3 velocity = glm::vec3(0, 40, 0);
public:
    Attack(
        glm::vec3 _pos = ZERO, 
        GLfloat _angle = 0, 
        glm::vec3 _axis = UP, 
        glm::vec3 _size = glm::vec3(1), 
        glm::vec3 _center = ZERO
    ) : Object(_pos, _angle, _axis, _size, _center) {

        if (load_mesh("assets/rice.obj")) {
            glm::mat4 meshTransform(1.0f);
            meshTransform = glm::scale(meshTransform, glm::vec3(0.3f));
            meshTransform = glm::rotate(meshTransform, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            meshTransform = glm::rotate(meshTransform, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            set_mesh_transform(meshTransform);
            set_mesh_color(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        }
    };

    void draw_shape() const override;
    void update(float deltaTime);

    int get_damage() { return damage; }
    void set_velocity(glm::vec3 v) { velocity = v; }
};