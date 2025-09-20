#include "bullet.h"

void Bullet::draw_shape() const {
    glColor3f(1, 0, 0);
    glutSolidSphere(0.8, 10, 10);
}

void Bullet::update(float deltaTime) {
    if (!get_isActive())
        return;
    translate(velocity * direction * deltaTime);
}