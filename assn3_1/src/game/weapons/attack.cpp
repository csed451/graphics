#include "attack.h"

void Attack::draw_shape() const {
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    GLfloat scaleFactor = 0.3f;
    
    glPushMatrix();
    glScalef(scaleFactor, scaleFactor, scaleFactor);    
    glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
    get_mesh()->draw();
    glPopMatrix();
}

void Attack::update(float deltaTime) {
    if (get_isActive())
        translate(velocity * deltaTime);
}