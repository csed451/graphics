#include "player.h"

void Player::draw_shape() const {
    glBegin(GL_TRIANGLES);
        glColor3f(0, 1, 1);
        glVertex3f(0, 1, 0);
        glVertex3f(-1, -1, 0);
        glVertex3f(1, -1, 0);
    glEnd();
}


void Player::shoot() {
    Attack* a = attackPool.acquire();
    a->init(get_pos());
}