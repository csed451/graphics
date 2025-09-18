
#include "object.h"
#include "player.h"
#include <iostream>

void myReshape (int w, int h);
void timer(int value);
void update(void);
void display (void);
void keyboard(unsigned char key, int x, int y);
void specialKeyboard(int key, int x, int y);

int prevTime = glutGet(GLUT_ELAPSED_TIME);

Player* player;

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
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard); 
    glutTimerFunc(0, timer, 0);

    player = new Player();

    /* start loop */
    glutMainLoop();
}

void myReshape (int w, int h) {
    glViewport (0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void timer(int value) {
    update();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void update(void) {
    int curTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (curTime - prevTime) / 1000.0f;
    prevTime = curTime;

    player->get_attackPool().update(deltaTime);
}

void display (void) {
    glClear (GL_COLOR_BUFFER_BIT);
    player->draw();
    player->get_attackPool().draw();
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    float step = 0.5f;
    switch (key) {
        case ' ':
            player->shoot();
            break;
    }
    glutPostRedisplay();  // 화면 다시 그리기 요청
}

void specialKeyboard(int key, int x, int y) {
    float step = 0.5f;
    switch (key) {
        case GLUT_KEY_UP:
            player->translate(glm::vec3(0, step,0));
            break;
        case GLUT_KEY_DOWN:
            player->translate(glm::vec3(0, -step,0));
        break;
        case GLUT_KEY_LEFT:
            player->translate(glm::vec3(-step, 0,0));
            break;
        case GLUT_KEY_RIGHT:
            player->translate(glm::vec3(step, 0,0));
            break;
    }
    glutPostRedisplay();  // 화면 다시 그리기 요청
}
