#include "bullet.h"
#include "core/render/renderer.h"
#include <glm/gtc/matrix_transform.hpp>

void Bullet::init_vertices(){
    for(int i=0; i <= segments; ++i){
        float a = (float)i / segments * TWO_PI;
        float ca = std::cos(a); // cosine of angle a
        float sa = std::sin(a); // sine of angle a
        outlineVertices.push_back(ca * (r + outline));
        outlineVertices.push_back(sa * (r + outline));
        outlineVertices.push_back(ca * r);
        outlineVertices.push_back(sa * r);
    }

    coreVertices.push_back(0.0f); 
    coreVertices.push_back(0.0f);    
    for (int i=0;i<=segments;++i){
        float a = (float)i / segments * TWO_PI;        
        coreVertices.push_back(std::cos(a)*r);
        coreVertices.push_back(std::sin(a)*r);
    }
}

void Bullet::draw_shape() const {
    const auto mesh = get_mesh();
    if (!mesh)
        return;

    glm::mat4 model = get_finalMatrix();
    model = glm::scale(model, glm::vec3(1.2f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));

    glm::vec4 color = counter
        ? glm::vec4(0.0f, 0.68f, 1.0f, 1.0f)
        : glm::vec4(1.0f, 0.32f, 0.0f, 1.0f);
    gRenderer.draw_mesh(*mesh, model, color);
}

void Bullet::update_logic(float deltaTime) {
    translate(velocity * direction * deltaTime);
}
