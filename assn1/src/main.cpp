
#include <GL/freeglut.h>


void myReshape (int w, int h);
void display (void);
void specialKeys(int key, int x, int y);
void drawModel();
void move(float x, float y);

float posX = 0.0f;
float posY = 0.0f;
GLfloat savedMatrix[16];


int main(int argc, char** argv)
{
    glutInit (&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100,100);
    glutCreateWindow("test");

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, savedMatrix);
    
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutSpecialFunc(specialKeys); 

    glutMainLoop();
}

void myReshape (int w, int h) {
    glViewport (0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity ();
    glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void display (void) {
    glClear (GL_COLOR_BUFFER_BIT);
    drawModel();
    glutSwapBuffers();
}

void specialKeys(int key, int x, int y) {
    float step = 0.5f;
    switch (key) {
        case GLUT_KEY_UP:
            move(0, step);
            break;
        case GLUT_KEY_DOWN:
            move(0, -step);
            break;
        case GLUT_KEY_LEFT:
            move(-step, 0);
            break;
        case GLUT_KEY_RIGHT:
            move(step, 0);
            break;
    }
    glutPostRedisplay();  // 화면 다시 그리기 요청
}

void drawModel() {
    glPushMatrix();
    glLoadMatrixf(savedMatrix);
    glColor3f(0.2f, 0.6f, 0.8f);     // 색상 설정
    glBegin(GL_QUADS);              // 사각형 그리기
        glVertex2f(-10.0f, -10.0f);
        glVertex2f( 10.0f, -10.0f);
        glVertex2f( 10.0f,  10.0f);
        glVertex2f(-10.0f,  10.0f);
    glEnd();
    glGetFloatv(GL_MODELVIEW_MATRIX, savedMatrix);
    glPopMatrix();
}

void move(float x, float y) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixf(savedMatrix);
    glTranslatef(x, y, 0.0f);  // 위치 이동
    glGetFloatv(GL_MODELVIEW_MATRIX, savedMatrix);
    glPopMatrix();
}