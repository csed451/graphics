#include "bullet.h"

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
    if (counter) 
        glColor4f(0.0f, 0.68f, 1.0f, 1.0f);
    else 
        glColor4f(1.0f, 0.32f, 0.0f, 1.0f);

    GLfloat scaleFactor = 1.2f;
    
    glPushMatrix();
    glScalef(scaleFactor, scaleFactor, scaleFactor);    
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
    get_mesh()->draw();
    glPopMatrix();
    
    return;
}

void Bullet::update(float deltaTime) {
    if (get_isActive())
        translate(velocity * direction * deltaTime);
}