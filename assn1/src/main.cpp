#include <iostream>
#include <cstdlib>

#include "globals.h"
#include "player.h"
#include "enemy.h"

enum class GameState { Playing, GameOver, Exiting };

GameState gameState = GameState::Playing;
Player* player = nullptr;
Enemy* enemy = nullptr;
int prevTime = 0;


void reshape (int w, int h);
void display (void);
void timer(int value);
void update(void);
void key_down(unsigned char key, int x, int y);
void special_key_down(int key, int x, int y);
void key_up(unsigned char key, int x, int y);
void special_key_up(int key, int x, int y);

void reset_game();

static void draw_ending_msg(const char* line1, const char* line2, int w, int h);
static void draw_game_over(const char* msg);
static void draw_stars();


int main(int argc, char** argv) {
    /* initial window setup */
    glutInit (&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Bullet Hell shooter");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* connect call back function */
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(key_down);
    glutSpecialFunc(special_key_down); 
    glutKeyboardUpFunc(key_up);
    glutSpecialUpFunc(special_key_up);
    glutTimerFunc(0, timer, 0);

    prevTime = glutGet(GLUT_ELAPSED_TIME);
    enemy = new Enemy(glm::vec3(0,30,0), 0, RIGHT, glm::vec3(2,2,2));
    player = new Player(glm::vec3(0,0,0), 0, UP, glm::vec3(2,2,2));

    glutMainLoop();
    
    delete enemy; 
    delete player; 
    
    return 0;
}

void reshape (int w, int h) {
    glViewport (0, 0, w, h);    
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f)));
    update_camera();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display (void) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    player->draw();
    enemy->draw();

    if (gameState == GameState::GameOver) {
        const char* msg = enemy->is_destroyed() ? "GAME WIN!" : "GAME OVER!";
        draw_game_over(msg);
        draw_stars();
    }
    
    glutSwapBuffers();
}

void timer(int value) {
    update();
    glutPostRedisplay();
    glutTimerFunc(1000.0f / FPS, timer, 0);
}

void update(void) {
    int curTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (curTime - prevTime) / 1000.0f;
    prevTime = curTime;

    if (gameState == GameState::Exiting) {
        glutLeaveMainLoop(); 
        return;
    }    
    if (gameState == GameState::GameOver) {
        if (cameraPos.z > 10) {
            cameraPos.z -= (100 - 10)/100.0f;
            cameraPos.y -= (10)/100.0f;
            update_camera();
        }
        else {
            if (enemy->is_destroyed()) {
                static float speed = 0.01f;
                static float cameraSpeed = 0.05f;
                cameraPos.y += cameraSpeed;
                cameraTarget = player->get_pos();
                player->translate(UP * speed);
                speed += 0.0001;
                if (speed > 0.05) {
                    speed += 0.001;
                    cameraSpeed += 0.001;
                    player->set_isAccelerating(true);
                }
                update_camera();
            }
            
        }
        return;
    }
    
    player->update(deltaTime, enemy);
    enemy->update(deltaTime, player);

    if (enemy->is_destroyed() || !player->get_isActive()) {
        gameState = GameState::GameOver;
        glm::vec3 pos = ZERO;
        if (enemy->is_destroyed()) {
            pos = player->get_pos();
        }
        else {
            pos = enemy->get_pos();
        }
        cameraTarget.x = pos.x;
        cameraTarget.y = pos.y + 5;
        cameraPos.x = pos.x;
    }
}

void key_down(unsigned char key, int x, int y) {
    if (gameState == GameState::GameOver) {
        if (key == 'r' || key == 'R') 
            reset_game();
        else if (key == 'q' || key == 'Q' || key == 27) 
            gameState = GameState::Exiting;
        return;
    }
    switch (key) {
        case ' ': 
            player->set_isShooting(true); 
            break;
        case 27: // ESC
            gameState = GameState::Exiting;
            break;
    }
}

void special_key_down(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            player->set_direction(UP);
            break;
        case GLUT_KEY_DOWN:
            player->set_direction(DOWN);
        break;
        case GLUT_KEY_LEFT:
            player->set_direction(LEFT);
            break;
        case GLUT_KEY_RIGHT:
            player->set_direction(RIGHT);
            break;
    }
}

void key_up(unsigned char key, int x, int y) {
    switch (key) {
        case ' ':
            player->set_isShooting(false);
            break;
    }
}

void special_key_up(int key, int x, int y) {
    glm::vec3 direction = player->get_direction();
    switch (key) {
        case GLUT_KEY_UP:
            if (direction == UP)
                player->set_direction(ZERO);
            break;
        case GLUT_KEY_DOWN:
            if (direction == DOWN)
                player->set_direction(ZERO);
            break;
        case GLUT_KEY_LEFT:
            if (direction == LEFT)
                player->set_direction(ZERO);
            break;
        case GLUT_KEY_RIGHT:
            if (direction == RIGHT)
                player->set_direction(ZERO);
            break;
    }
}

void reset_game() {
    prevTime = glutGet(GLUT_ELAPSED_TIME);

    enemy->reset();
    player->reset();        
    gameState = GameState::Playing;

    glutPostRedisplay();
}

static void draw_ending_msg(const char* line1, const char* line2, int w, int h) {
    auto textWidth = [&](const char* s) -> int {
        return glutBitmapLength(GLUT_BITMAP_HELVETICA_18,
                                reinterpret_cast<const unsigned char*>(s));
    };
    auto drawText = [&](float x, float y, const char* s) {
        glRasterPos2f(x, y);
        while (*s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *s++);
    };

    int width1 = textWidth(line1);
    int width2 = textWidth(line2);

    const float fontH = 18.0f;
    const float gap   = 10.0f;

    float line1Y = h * 0.5f + (fontH * 0.5f + gap * 0.5f);
    float line2Y = h * 0.5f - (fontH * 0.5f + gap * 0.5f);
    float line1X = (w - width1) * 0.5f;
    float line2X = (w - width2) * 0.5f;

    glColor3f(1,1,1);
    drawText(line1X, line1Y, line1);
    drawText(line2X, line2Y, line2);
}

static void draw_game_over(const char* msg) {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Translucent gray background
    glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
    glBegin(GL_QUADS);
        glVertex2f(0,0);
        glVertex2f(w,0);
        glVertex2f(w,h);
        glVertex2f(0,h);
    glEnd();

    draw_ending_msg(msg, "Press R to Restart / Q to Quit", w, h);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

static void draw_stars() {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixf(glm::value_ptr(cameraMatrix));

    srand(42);
    glBegin(GL_POINTS);
    for (int i = 0; i < 100000; ++i) {
        int x = rand() % 1000 - 500;
        int y = rand() % 1000 - 500;
        int z = rand() % 1000 - 500;
        glColor3f(1.0f, 1.0f, 1.0f); // 흰색 별
        glVertex3f(x, y, z);
    }
    glEnd();

    glPopMatrix();
}