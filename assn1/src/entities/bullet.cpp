#include "bullet.h"

void Bullet::draw_shape() const {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);    
    glDisable(GL_CULL_FACE);

    const float r = 1.6f;          
    const float outline = 0.6f;    
    const int segments = 48;

    // 외곽 링 색상 번갈아 적용
    if (counter) 
        glColor3f(0.0f, 0.68f, 1.0f); 
    else 
        glColor3f(1.0f, 0.32f, 0.0f);         
    
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float a = (float)i / segments * TWO_PI;
        float ca = std::cos(a);
        float sa = std::sin(a);
        glVertex2f(ca * (r + outline), sa * (r + outline)); 
        glVertex2f(ca * r, sa * r);             
    }
    glEnd();

    // 내부 흰색 원
    glColor3f(1,1,1);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0,0);
        for (int i=0;i<=segments;++i){
            float a = (float)i / segments * TWO_PI;
            glVertex2f(std::cos(a)*r, std::sin(a)*r);
        }
    glEnd();

    glPopAttrib();
}

void Bullet::update(float deltaTime) {
    if (!get_isActive())
        return;
    translate(velocity * direction * deltaTime);
}