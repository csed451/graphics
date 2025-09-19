#include "globals.h"
#include "player.h"



void Player::draw_shape() const {
    glBegin(GL_TRIANGLES);
        glColor3f(0, 1, 1);
        glVertex3f(0, 1, 0);
        glVertex3f(-1, -1, 0);
        glVertex3f(1, -1, 0);
    glEnd();
}


void Player::update(float deltaTime) {
    if (!get_isActive())
        return; 
    translate(velocity * direction * deltaTime);
    if (is_outside_window(get_pos()))
        translate(- velocity * direction * deltaTime);

    if (heart <= 0)
        set_isActive(false);
    leftCanon.update(deltaTime);
    rightCanon.update(deltaTime);
    attackPool.update(deltaTime);
}

void Player::draw() const {
    Object::draw();
    leftCanon.draw();
    rightCanon.draw();
    attackPool.draw();
}


void Player::shoot() {
    int curTime = glutGet(GLUT_ELAPSED_TIME);
    if (1000.0f / rps < curTime - lastShootTime) {
        Attack* a = attackPool.acquire();
        a->init(get_pos(), 0, glm::vec3(1,0,0), glm::vec3(2,2,2));
        lastShootTime = curTime;
    }
}