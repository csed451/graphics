#pragma once

#include "core/base/object.h"
#include "core/render/renderer.h"
#include <glm/gtc/matrix_transform.hpp>

class Orbit : public Object {
private:
    float velocity = 90.0f;
public:
    Orbit(
        glm::vec3 _pos=ZERO, 
        GLfloat _angle=0, 
        glm::vec3 _axis=UP, 
        glm::vec3 _size=glm::vec3(1), 
        glm::vec3 _center=ZERO
    ) : Object(_pos, _angle, _axis, _size, _center) {
        set_mesh(load_mesh("assets/star.obj"));     
    };

    void draw_shape() const override {
        const auto mesh = get_mesh();
        if (!mesh)
            return;

        glm::mat4 model = get_finalMatrix();
        model = glm::scale(model, glm::vec3(0.8f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));

        gRenderer.draw_mesh(*mesh, model, glm::vec4(0.85f, 0.15f, 0.15f, 1.0f));
    }

    void update_logic(float deltaTime) override {
        rotate_world(velocity * deltaTime, FORWARD);
    }
};
