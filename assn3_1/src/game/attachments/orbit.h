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
        set_mesh(load_mesh("assets/star.obj"));     
    };

    void draw_shape() const override {
        glColor4f(0.85f, 0.15f, 0.15f, 1.0f);
        GLfloat scaleFactor = 0.8f;
        
        glPushMatrix();
        glScalef(scaleFactor, scaleFactor, scaleFactor);    
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
        get_mesh()->draw();
        glPopMatrix();
    }

    void update(float deltaTime) {
        if (!get_isActive())
            return;
        rotate_world(velocity * deltaTime, FORWARD);
    }
};
