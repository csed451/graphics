#pragma once

#include "object.h"

class Heart : public Object {
public:
    Heart(glm::vec3 _pos=ZERO, 
        GLfloat _angle=0, 
        glm::vec3 _axis=UP,  
        glm::vec3 _size=glm::vec3(1), 
        glm::vec3 _center=ZERO) 
        : Object(_pos, _angle, _axis, _size, _center) {};

    void draw_shape() const override {
        glColor3f(1, 0, 0);
        glBegin(GL_POLYGON);
            glVertex3f(0.0f, -0.5f, 0);   // 하단 뾰족한 부분
            glVertex3f(-0.5f, 0.0f, 0);   // 좌측 아래
            glVertex3f(-0.7f, 0.4f, 0);   // 좌측 곡선
            glVertex3f(-0.4f, 0.7f, 0);   // 좌측 상단
            glVertex3f(0.0f, 0.6f, 0);    // 중앙 상단
            glVertex3f(0.4f, 0.7f, 0);    // 우측 상단
            glVertex3f(0.7f, 0.4f, 0);    // 우측 곡선
            glVertex3f(0.5f, 0.0f, 0);    // 우측 아래
        glEnd();
    }
};