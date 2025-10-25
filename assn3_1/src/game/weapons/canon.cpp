#include "canon.h"
#include <glm/gtc/quaternion.hpp>

void Canon::draw_shape() const {
    glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
    
    glScalef(0.3f, 0.3f, 0.3f);
    get_mesh()->draw();
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
