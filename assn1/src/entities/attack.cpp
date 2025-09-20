
#include "attack.h"

void Attack::draw_shape() const {
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
        glVertex3f(-0.2, -1, 0);
        glVertex3f(0.2, -1, 0);
        glVertex3f(0.2, 1, 0);
        glVertex3f(-0.2, 1, 0);
    glEnd();
}

void Attack::update(float deltaTime) {
    if (!get_isActive())
        return;
    translate(velocity * deltaTime);
}