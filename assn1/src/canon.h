
#include "object.h"


class Canon : public Object {
public:
    Canon(glm::vec3 _pos=glm::vec3(), GLfloat _angle=0, glm::vec3 _axis=glm::vec3(1,0,0), glm::vec3 _size=glm::vec3(1), glm::vec3 _center=glm::vec3(0, -0.5, 0)) : Object(_pos, _angle, _axis, _size, _center) {};

    void draw_shape() const override {
        glColor3f(0.5, 0.5, 0.5);
        glBegin(GL_QUADS);
            glVertex3f(-0.2, -0.5, 0);
            glVertex3f(0.2, -0.5, 0);
            glVertex3f(0.2, 0.5, 0);
            glVertex3f(-0.2, 0.5, 0);
        glEnd();
    }
    void update(float deltaTime){ rotate_local(1, glm::vec3(0,0,1));}
};