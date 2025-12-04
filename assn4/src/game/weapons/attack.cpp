#include "attack.h"
#include "core/render/renderer.h"
#include <glm/gtc/matrix_transform.hpp>

void Attack::draw_shape() const {
    const auto mesh = get_mesh();
    if (!mesh)
        return;

    glm::mat4 model = get_finalMatrix();
    model = glm::scale(model, glm::vec3(0.3f));
    gRenderer.draw_mesh(*mesh, model, get_prevModelMatrix(), glm::vec4(1.0f));
}

void Attack::update_logic(float deltaTime) {
    translate(velocity * deltaTime);
}
