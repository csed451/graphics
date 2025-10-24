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
    glColor4f(1.0f, 0.8f, 0.8, 1.0f);
    GLfloat scaleFactor = 1.2f;
    
    glPushMatrix();
    glScalef(scaleFactor, scaleFactor, scaleFactor);    
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
    get_mesh()->draw();
    glPopMatrix();
    
    return;
        
    // glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);    
    // glDisable(GL_CULL_FACE);
    // glEnableClientState(GL_VERTEX_ARRAY);
    
    // if (counter) 
    //     glColor3f(0.0f, 0.68f, 1.0f);
    // else 
    //     glColor3f(1.0f, 0.32f, 0.0f);
    
    // // outline Vertices
    // glVertexPointer(2, GL_FLOAT, 0, outlineVertices.data());
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, outlineVertices.size() / 2);

    // // core Vertices
    // glColor3f(1,1,1);
    // glVertexPointer(2, GL_FLOAT, 0, coreVertices.data());
    // glDrawArrays(GL_TRIANGLE_FAN, 0, coreVertices.size() / 2);
    
    // glDisableClientState(GL_VERTEX_ARRAY);
    // glPopAttrib();
}

void Bullet::update(float deltaTime) {
    if (get_isActive())
        translate(velocity * direction * deltaTime);
}