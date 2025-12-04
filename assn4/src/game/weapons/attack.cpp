#include "attack.h"
#include "core/render/renderer.h"
#include <glm/gtc/matrix_transform.hpp>

void Attack::draw_shape() const {
    const auto mesh = get_mesh();
    if (!mesh)
        return;

    glm::mat4 model = get_finalMatrix();
    glm::mat4 prevModel = get_prevModelMatrix();
    model = glm::scale(model, glm::vec3(0.3f));
    prevModel = glm::scale(prevModel, glm::vec3(0.3f));
    gRenderer.draw_mesh(*mesh, model, prevModel, glm::vec4(1.0f));
}

void Attack::update_logic(float deltaTime) {
    translate(velocity * deltaTime);
}
