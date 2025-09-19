#include "globals.h"
#include "object.h"
#include "player.h"
#include <iostream>

void myReshape (int w, int h);
void timer(int value);
void update(void);
void display (void);
void key_down(unsigned char key, int x, int y);
void special_key_down(int key, int x, int y);
void key_up(unsigned char key, int x, int y);
void special_key_up(int key, int x, int y);

int prevTime = glutGet(GLUT_ELAPSED_TIME);


Player* player = nullptr;

class DummyEnemy : public Object{
private:
    int heart = 5;
public:
    DummyEnemy(glm::vec3 _pos=glm::vec3(), GLfloat _angle=0, glm::vec3 _axis=glm::vec3(1,0,0), glm::vec3 _size=glm::vec3(1), glm::vec3 _center=glm::vec3()) : Object(_pos, _angle, _axis, _size, _center) {};

    void draw_shape() const override;
    void update(float deltaTime);
};

void DummyEnemy::draw_shape() const {
    glColor3f(1, 0, 1);
    glBegin(GL_QUADS);
        glVertex3f(-1, -1, 0);
        glVertex3f(1, -1, 0);
        glVertex3f(1, 1, 0);
        glVertex3f(-1, 1, 0);
    glEnd();
}

void DummyEnemy::update(float deltaTime) {
    ObjectPool<Attack>& pool = player->get_attackPool();
    for (auto obj : pool.get_pool()) {
        if (obj->get_isActive()) {
            float aR = obj->get_hitboxRadius();
            float eR = get_hitboxRadius();
            float distance = glm::distance(obj->get_pos(), get_pos());
            if (aR + eR >= distance) {
                heart -= 1;
                std::cout << heart << std::endl;
                pool.release(obj);
            }
        }
    }
}

DummyEnemy* enemy = nullptr;

int main(int argc, char** argv)
{
    /* initial window setup */
    glutInit (&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100,100);
    glutCreateWindow("player");

    /* connect call back function */
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(key_down);
    glutSpecialFunc(special_key_down); 
    glutKeyboardUpFunc(key_up);
    glutSpecialUpFunc(special_key_up);
    glutTimerFunc(0, timer, 0);

    enemy = new DummyEnemy();
    enemy->init(glm::vec3(0,30,0), 0, glm::vec3(1,0,0), glm::vec3(2,2,2));
    player = new Player();
    player->init(glm::vec3(0,0,0), 0, glm::vec3(1,0,0), glm::vec3(2,2,2));

    /* start loop */
    glutMainLoop();
}

void myReshape (int w, int h) {
    glViewport (0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(orthoLeft, orthoRight, orthoBottom, orthoTop, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void timer(int value) {
    update();
    glutPostRedisplay();
    glutTimerFunc(6.94, timer, 0);
}

void update(void) {
    int curTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (curTime - prevTime) / 1000.0f;
    prevTime = curTime;

    player->update(deltaTime);
    enemy->update(deltaTime);
}

void display (void) {
    glClear (GL_COLOR_BUFFER_BIT);
    player->draw();
    player->get_attackPool().draw();
    enemy->draw();
    glutSwapBuffers();
}

void key_down(unsigned char key, int x, int y) {
    switch (key) {
        case ' ':
            player->shoot();
            break;
    }
}

void special_key_down(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            player->set_direction(glm::vec3(0, 1, 0));
            break;
        case GLUT_KEY_DOWN:
            player->set_direction(glm::vec3(0, -1, 0));
        break;
        case GLUT_KEY_LEFT:
            player->set_direction(glm::vec3(-1, 0, 0));
            break;
        case GLUT_KEY_RIGHT:
            player->set_direction(glm::vec3(1, 0, 0));
            break;
    }
}

void key_up(unsigned char key, int x, int y) {

}

void special_key_up(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
            player->set_direction(glm::vec3(0, 0, 0));
            break;
    }
}

