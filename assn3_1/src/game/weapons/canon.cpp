#include "canon.h"
#include <glm/gtc/quaternion.hpp>

void Canon::draw_shape() const {
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_QUADS);
        glVertex3f(-0.2, -0.5, 0);
        glVertex3f(0.2, -0.5, 0);
        glVertex3f(0.2, 0.5, 0);
        glVertex3f(-0.2, 0.5, 0);
    glEnd();
}

void Canon::update(float deltaTime) {
    if (!get_isActive())
        return; 
    attackPool.update(deltaTime);
    shootCooldown -= deltaTime;
}

void Canon::shoot() {
    if (shootCooldown <= 0) {
        Attack* a = attackPool.acquire();
        a->init(get_pos(), glm::degrees(glm::angle(get_quat())), glm::axis(get_quat()), glm::vec3(1));
        shootCooldown = shootInterval;
    }
}
