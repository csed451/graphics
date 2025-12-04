#pragma once

#include <GL/glew.h>
#include "core/base/object.h"
#include "core/render/renderer.h"
#include <glm/gtc/matrix_transform.hpp>

class Orbit : public Object {
private:
    float velocity = 90.0f;
    mutable GLuint diffuseTex = 0;
    mutable GLuint normalTex = 0;
    mutable bool hasNormalMap = false;
public:
    Orbit(
        glm::vec3 _pos=ZERO, 
        GLfloat _angle=0, 
        glm::vec3 _axis=UP, 
        glm::vec3 _size=glm::vec3(1), 
        glm::vec3 _center=ZERO
    ) : Object(_pos, _angle, _axis, _size, _center) {
        set_mesh(load_mesh("assets/models/sphere.obj"));     
    };

    void draw_shape() const override {
        const auto mesh = get_mesh();
        if (!mesh)
            return;

        glm::mat4 model = get_finalMatrix();
        glm::mat4 prevModel = get_prevModelMatrix();

        model = glm::scale(model, glm::vec3(0.8f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));
        prevModel = glm::scale(prevModel, glm::vec3(0.8f));
        prevModel = glm::rotate(prevModel, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        prevModel = glm::rotate(prevModel, glm::radians(180.0f), glm::vec3(0, 0, 1));

        if (diffuseTex == 0)
            diffuseTex = gRenderer.get_or_load_texture("assets/textures/diffuse_primary.png");
        if (normalTex == 0) {
            normalTex = gRenderer.get_or_load_texture("assets/textures/normal_organic.png");
            hasNormalMap = (normalTex != 0);
        }

        // Use white tint so texture shows its original colors
        gRenderer.draw_mesh(*mesh, model, prevModel, glm::vec4(1.0f), true, diffuseTex, normalTex, hasNormalMap);
    }

    void update_logic(float deltaTime) override {
        rotate_world(velocity * deltaTime, FORWARD);
    }
};
