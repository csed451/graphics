#include <iostream>

#include "globals.h"
#include "object.h"
#include "player.h"
#include "enemy.h"

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
Enemy* enemy = nullptr;


int main(int argc, char** argv) {
    /* initial window setup */
    glutInit (&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Bullet Hell shooter");

    /* connect call back function */
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(key_down);
    glutSpecialFunc(special_key_down); 
    glutKeyboardUpFunc(key_up);
    glutSpecialUpFunc(special_key_up);
    glutTimerFunc(0, timer, 0);

    enemy = new Enemy();
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


    // player->update(deltaTime, enemy->get_bulletPool()); 로 수정해야 함
    player->update(deltaTime);
    enemy->update(deltaTime, player->get_attackPool());

}

void display (void) {
    glClear (GL_COLOR_BUFFER_BIT);
    player->draw();
    player->get_attackPool().draw();
    enemy->draw();
    enemy->get_bulletPool().draw();
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

