#pragma once

#include <vector>
#include "core/base/object.h"

class Bullet : public Object {
private:
    std::vector<float> outlineVertices;
    std::vector<float> coreVertices;         
    void init_vertices();
    
    // Bullet properties
    const float outline = 0.6f;    
    const int segments = 48;
    const float r = 1.6f;  

    mutable bool counter = false; 
    glm::vec3 direction = DOWN;
    float velocity = 20.0f;
public:
    Bullet(
        glm::vec3 _pos=glm::vec3(), 
        GLfloat _angle=0, 
        glm::vec3 _axis=RIGHT, 
        glm::vec3 _size=glm::vec3(1), 
        glm::vec3 _center=ZERO
    ) : Object(_pos, _angle, _axis, _size, _center) { 
        set_hitboxRadius(r + outline);
        init_vertices();        
        set_mesh(load_mesh("assets/sphere.obj"));
    };
    
    void draw_shape() const override;
    void update_logic(float deltaTime) override;

    void set_direction(glm::vec3 dir) { direction = dir; }
    void set_counter(bool c) { counter = c; }
};