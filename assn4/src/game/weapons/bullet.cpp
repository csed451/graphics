#include "bullet.h"
#include "core/render/renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "core/render/mesh.h"

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

    ensure_sonic_child();

    glm::mat4 model = get_finalMatrix();
    model = glm::scale(model, glm::vec3(0.7f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));

    gRenderer.draw_mesh(*mesh, model, glm::vec4(1.0f)); // bullet always white

    if (sonicMesh) {
        glm::vec3 forward = glm::length(direction) > 1e-5f ? glm::normalize(direction) : glm::vec3(0, 0, 1);
        glm::vec3 up = glm::vec3(0, 0, 1);
        if (std::abs(glm::dot(forward, up)) > 0.99f)
            up = glm::vec3(0, 1, 0);
        glm::vec3 right = glm::normalize(glm::cross(up, forward));
        up = glm::normalize(glm::cross(forward, right));

        glm::mat4 basis(1.0f);
        basis[0] = glm::vec4(right, 0.0f);
        basis[1] = glm::vec4(up,    0.0f);
        basis[2] = glm::vec4(forward, 0.0f);

        glm::mat4 childModel = get_finalMatrix();
        childModel = childModel * basis;
        childModel = glm::translate(childModel, glm::vec3(0.0f, 0.0f, 2.0f));
        childModel = glm::rotate(childModel, glm::radians(90.0f), glm::vec3(0, 0, 1));
        childModel = glm::scale(childModel, glm::vec3(0.7f));

        GLuint sonicTex = counter ? sonicDiffuse2 : sonicDiffuse1;
        if (sonicTex == 0) // fallback if one texture failed to load
            sonicTex = counter ? sonicDiffuse1 : sonicDiffuse2;
        gRenderer.draw_mesh(*sonicMesh, childModel, glm::vec4(1.0f), true, sonicTex, sonicNormal, sonicHasNormal);
    }
}

void Bullet::update_logic(float deltaTime) {
    translate(velocity * direction * deltaTime);
}

// Factory-style initializer called after Bullet creation
void Bullet::ensure_sonic_child() const {
    if (!sonicMesh)
        sonicMesh = load_mesh("assets/models/sonic.obj");
    if (sonicDiffuse1 == 0)
        sonicDiffuse1 = gRenderer.get_or_load_texture("assets/textures/diffuse_sonic_1.png");
    if (sonicDiffuse2 == 0)
        sonicDiffuse2 = gRenderer.get_or_load_texture("assets/textures/diffuse_sonic_2.png");
    if (sonicNormal == 0) {
        sonicNormal = gRenderer.get_or_load_texture("assets/textures/normal_flat.png");
        sonicHasNormal = (sonicNormal != 0);
    }
}
