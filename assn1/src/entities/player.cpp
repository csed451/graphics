#include "globals.h"
#include "player.h"

void Player::draw_shape() const {
    glColor3f(0, 1, 1);
    glBegin(GL_TRIANGLES);
        glVertex3f(0, 1, 0);
        glVertex3f(-1, -1, 0);
        glVertex3f(1, -1, 0);
    glEnd();
    if(!isShooting && direction != glm::vec3()) {
        glColor3f(1, 0, 0);
        glBegin(GL_QUADS);
            glVertex3f(-1.2, -1.8, 0);
            glVertex3f(-0.3, -1.8, 0);
            glVertex3f(-0.5, -1, 0);
            glVertex3f(-1, -1, 0);
        glEnd();
        glBegin(GL_QUADS);
            glVertex3f(0.3, -1.8, 0);
            glVertex3f(1.2, -1.8, 0);
            glVertex3f(1, -1, 0);
            glVertex3f(0.5, -1, 0);
        glEnd();
    }
}

void Player::update(float deltaTime) {
    if (!get_isActive())
        return; 
    glm::vec3 move = velocity * direction * deltaTime;
    move *= (isShooting ? 0.5 : 1);
    translate(move);
    if (is_outside_window(get_pos()))
        translate(-move);

    if (heart <= 0)
        set_isActive(false);

    if (isShooting) {
        leftCanon.shoot();
        rightCanon.shoot();
    }
    leftCanon.update(deltaTime);
    rightCanon.update(deltaTime);
}

void Player::draw() const {
    Object::draw();
    leftCanon.draw();
    rightCanon.draw();
}