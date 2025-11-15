#include "canon.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/render/renderer.h"

void Canon::draw_shape() const {
    const auto mesh = get_mesh();
    if (!mesh)
        return;

    glm::mat4 model = get_finalMatrix();
    model = glm::scale(model, glm::vec3(0.3f));
    gRenderer.draw_mesh(*mesh, model, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
}

void Canon::update_logic(float deltaTime) {
    shootCooldown -= deltaTime;
}

void Canon::shoot() {
    if (shootCooldown <= 0) {
        Attack* a = attackPool.acquire();
        a->init(get_pos(), glm::degrees(glm::angle(get_quat())), glm::axis(get_quat()), glm::vec3(1));
        shootCooldown = shootInterval;
    }
}
