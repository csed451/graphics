#pragma once

#include "core/base/object.h"

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
        if (load_mesh("assets/star.obj")) {
            glm::mat4 meshTransform(1.0f);
            meshTransform = glm::scale(meshTransform, glm::vec3(0.8f));
            meshTransform = glm::rotate(meshTransform, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            meshTransform = glm::rotate(meshTransform, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            set_mesh_transform(meshTransform);
            set_mesh_color(glm::vec4(0.85f, 0.15f, 0.15f, 1.0f));
        }        
    };

    void draw_shape() const override {
        if(get_mesh())
            return;

        glColor3f(1, 0, 0);
        glBegin(GL_LINE_LOOP); // 또는 GL_TRIANGLE_FAN

        for (int i = 0; i < 64; i++) {
            float theta = TWO_PI * i / 64;
            float x = 0.5 * cosf(theta);
            float y = 0.5 * sinf(theta);
            glVertex3f(x, y, 0);
        }

        glEnd();

    }

    void update(float deltaTime) {
        if (!get_isActive())
            return;
        rotate_world(velocity * deltaTime, FORWARD);
    }
};
