
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

void myReshape (int w, int h);
void display (void);
void keyboard(unsigned char key, int x, int y);
void specialKeyboard(int key, int x, int y);


class Object {
private:
    glm::mat4 modelMatrix;
    glm::vec3 center;
    bool isLocal;       // default: true
    Object* parent;
public:
    Object(glm::vec3 _center = glm::vec3(), glm::vec3 _pos = glm::vec3(), glm::vec3 _size = glm::vec3(1), GLfloat _angle=0, glm::vec3 _axis=glm::vec3(1,0,0));
    /* getter */
    glm::mat4 get_modelMatrix() const { return modelMatrix; }
    glm::vec3 get_center() const { return center; }
    bool get_isLocal() const { return isLocal; }
    Object* get_parent() const { return parent; }
    /* additional information */
    glm::vec3 get_pos() const;
    glm::vec3 get_size() const;
    glm::quat get_quat() const;
    glm::mat4 get_finalMatrix() const;
    /* setter */
    void set_local(bool b);
    void set_center(glm::vec3 v);
    void set_parent(Object* _parent);

    void translate(glm::vec3 v);
    void rotate(GLfloat angle, glm::vec3 axis);
    void scale(glm::vec3 v);
    // defined in local coordinate system
    void translate_local(glm::vec3 v);
    void rotate_local(GLfloat angle, glm::vec3 v);
    void scale_local(glm::vec3 v);
    // defined in world coordinate system
    void translate_world(glm::vec3 v);
    void rotate_world(GLfloat angle, glm::vec3 axis);
    void scale_world(glm::vec3 v);

    void draw() const;
    virtual void draw_shape() const = 0;
};

Object::Object(glm::vec3 _center, glm::vec3 _pos, glm::vec3 _size, GLfloat _angle, glm::vec3 _axis)
: modelMatrix(glm::mat4(1.0f)), center(_center), isLocal(true), parent(nullptr) {
    translate_world(_pos);
    scale_local(_size);
    rotate_local(_angle, _axis);
}

glm::vec3 Object::get_pos() const {
    return glm::vec3(get_finalMatrix()[3]);
}

glm::vec3 Object::get_size() const {
    glm::mat3 rotationMatrix = glm::mat3(get_finalMatrix());
    GLfloat scaleX = glm::length(rotationMatrix[0]);
    GLfloat scaleY = glm::length(rotationMatrix[1]);
    GLfloat scaleZ = glm::length(rotationMatrix[2]);
    return glm::vec3(scaleX, scaleY, scaleZ);
}

glm::quat Object::get_quat() const {
    // extract only a direction of modelMatrix
    glm::mat3 rotationMatrix = glm::mat3(get_finalMatrix());
    rotationMatrix[0] = glm::normalize(rotationMatrix[0]);
    rotationMatrix[1] = glm::normalize(rotationMatrix[1]);
    rotationMatrix[2] = glm::normalize(rotationMatrix[2]);

    return glm::quat_cast(rotationMatrix);
}

glm::mat4 Object::get_finalMatrix() const {
    return parent ? parent->get_modelMatrix() * modelMatrix : modelMatrix;
}

void Object::set_local(bool b) {
    isLocal = b;
}

void Object::set_center(glm::vec3 v) {
    center = v;
}

void Object::set_parent(Object* _parent) {
    if (_parent) {
        modelMatrix = glm::inverse(_parent->get_modelMatrix()) * modelMatrix;
        isLocal = false;
    }
    else{
        modelMatrix = get_finalMatrix();
        isLocal = true;
    }

    parent = _parent;
}

void Object::translate(glm::vec3 v) {
    if (isLocal) 
        translate_local(v);
    else
        translate_world(v);
}

void Object::rotate(GLfloat angle, glm::vec3 axis) {
    if (isLocal) 
        rotate_local(angle, axis);
    else
        rotate_world(angle, axis);
}

void Object::scale(glm::vec3 v) {
    if (isLocal) 
        scale_local(v);
    else
        scale_world(v);
}

void Object::translate_local(glm::vec3 v) {
    modelMatrix = glm::translate(modelMatrix, v);
}

void Object::rotate_local(GLfloat angle, glm::vec3 axis) {
    translate_local(center);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), axis);
    translate_local(-center);
}

void Object::scale_local(glm::vec3 v) {
    translate(center);
    modelMatrix = glm::scale(modelMatrix, v);
    translate(-center);
}

void Object::translate_world(glm::vec3 v) {
    glm::mat4 mT = glm::translate(glm::mat4(1.0f), v);
    modelMatrix = mT * modelMatrix;
}

void Object::rotate_world(GLfloat angle, glm::vec3 axis) {
    glm::mat4 mR = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
    modelMatrix = mR * modelMatrix;
}

void Object::scale_world(glm::vec3 v) {
    glm::mat4 mS = glm::scale(glm::mat4(1.0f), v);
    modelMatrix = mS * modelMatrix;
}

void Object::draw() const {
    glPushMatrix();
    glLoadMatrixf(glm::value_ptr(get_finalMatrix()));

    draw_shape();

    glPopMatrix();
}

class Player : public Object {
public:
    Player(glm::vec3 _center = glm::vec3(), glm::vec3 _pos = glm::vec3(), glm::vec3 _size = glm::vec3(1), GLfloat _angle=0, glm::vec3 _axis=glm::vec3(1,0,0)) : Object(_center, _pos, _size, _angle, _axis) {};

    void draw_shape() const override;
};

void Player::draw_shape() const {
    glBegin(GL_TRIANGLES);
        glColor3f(0, 1, 1);
        glVertex3f(0, 1, 0);
        glVertex3f(-1, -1, 0);
        glVertex3f(1, -1, 0);
    glEnd();
}

class Father : public Object {
public:
    Father(glm::vec3 _center = glm::vec3(), glm::vec3 _pos = glm::vec3(), glm::vec3 _size = glm::vec3(1), GLfloat _angle=0, glm::vec3 _axis=glm::vec3(1,0,0)) : Object(_center, _pos, _size, _angle, _axis) {};

    void draw_shape() const override;
};

void Father::draw_shape() const {
    glBegin(GL_TRIANGLES);
        glColor3f(1, 0, 0);
        glVertex3f(0, 1, 0);
        glVertex3f(-1, -1, 0);
        glVertex3f(1, -1, 0);
    glEnd();
}

Father father(glm::vec3(0, 0, 0), glm::vec3(10,10,0), glm::vec3(1,1,1));
Player player(glm::vec3(5, 0, 0), glm::vec3(25,25,0), glm::vec3(3,3,1));


int main(int argc, char** argv)
{
    glutInit (&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100,100);
    glutCreateWindow("test");

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    player.set_parent(&father);
    father.set_local(false);

    
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard); 


    glutMainLoop();
}

void myReshape (int w, int h) {
    glViewport (0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity ();
    glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void display (void) {
    glClear (GL_COLOR_BUFFER_BIT);
    player.draw();
    father.draw();
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    float step = 0.5f;
    switch (key) {
        case ' ':
            player.rotate(45.0, glm::vec3(0,0,1));
            break;
        case 'w':
            father.translate(glm::vec3(0, step,0));
            break;
        case 'a':
            father.translate(glm::vec3(-step, 0,0));
            break;
        case 's':
            father.translate(glm::vec3(0, -step,0));
            break;
        case 'd':
            father.translate(glm::vec3(step, 0,0));
            break;
        case 'r':
            father.rotate(45.0, glm::vec3(0,0,1));
            break;
    }
    glutPostRedisplay();  // 화면 다시 그리기 요청
}

void specialKeyboard(int key, int x, int y) {
    float step = 0.5f;
    switch (key) {
        case GLUT_KEY_UP:
            player.translate(glm::vec3(0, step,0));
            break;
        case GLUT_KEY_DOWN:
            player.translate(glm::vec3(0, -step,0));
        break;
        case GLUT_KEY_LEFT:
            player.translate(glm::vec3(-step, 0,0));
            break;
        case GLUT_KEY_RIGHT:
            player.translate(glm::vec3(step, 0,0));
            break;
    }
    glutPostRedisplay();  // 화면 다시 그리기 요청
}
