#include <iostream>

#include "globals.h"
#include "player.h"
#include "enemy.h"

enum class GameState { Playing, GameOver, Exiting };

GameState gameState = GameState::Playing;
Player* player = nullptr;
Enemy* enemy = nullptr;
std::vector<Star> stars;
int prevTime = 0;


void reshape (int w, int h);
void display (void);
void timer(int value);
void update(void);
void key_down(unsigned char key, int x, int y);
void special_key_down(int key, int x, int y);
void key_up(unsigned char key, int x, int y);
void special_key_up(int key, int x, int y);

void init_stars();
void reset_game();

static void draw_ending_msg(const char* line1, const char* line2, int w, int h);
static void draw_game_over(const char* msg);


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

    init_stars();
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
    glLoadIdentity();
    glOrtho(ORTHO_LEFT, ORTHO_RIGHT, ORTHO_BOTTOM, ORTHO_TOP, -50.0, 50.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display (void) {
    glClear (GL_COLOR_BUFFER_BIT);

    glPushMatrix();
    glm::mat4 mvp = CAMERA_METRIX;
    glLoadMatrixf(glm::value_ptr(mvp));
    for (const auto& star : stars) {
        glPushMatrix();
        glTranslatef(star.pos.x, star.pos.y, star.pos.z);
        glColor3f(star.color.r, star.color.g, star.color.b);
        glutSolidSphere(star.size, 8, 8);
        glPopMatrix();
    }
    glPopMatrix();

    player->draw();
    enemy->draw();
    if (gameState == GameState::GameOver) {
        const char* msg = enemy->is_destroyed() ? "GAME WIN!" : "GAME OVER!";
        draw_game_over(msg);
    }
    
    glutSwapBuffers();
}

void timer(int value) {
    update();
    glutPostRedisplay();
    glutTimerFunc(1000 / FPS, timer, 0);
}

void update(void) {
    int curTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (curTime - prevTime) / 1000.0f;
    prevTime = curTime;

    if (gameState == GameState::Exiting) {
        glutLeaveMainLoop(); 
        return;
    }    
    if (gameState == GameState::GameOver)
        return;

    // Update stars
    const float STAR_SCROLL_SPEED = 10.0f;
    for (auto& star : stars) {
        star.pos.y -= STAR_SCROLL_SPEED * deltaTime;
        if (star.pos.y < ORTHO_BOTTOM) {
            star.pos.y = ORTHO_TOP;
            star.pos.x = (rand() / (float)RAND_MAX) * (ORTHO_RIGHT - ORTHO_LEFT) + ORTHO_LEFT;
        }
    }
    
    player->update(deltaTime, enemy);
    enemy->update(deltaTime, player);

    if (enemy->is_destroyed() || !player->get_isActive()) 
        gameState = GameState::GameOver;
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

void init_stars() {
    stars.resize(NUM_STARS);
    for (int i = 0; i < NUM_STARS; ++i) {
        stars[i].pos.x = (rand() / (float)RAND_MAX) * (ORTHO_RIGHT - ORTHO_LEFT) + ORTHO_LEFT;
        stars[i].pos.y = (rand() / (float)RAND_MAX) * (ORTHO_TOP - ORTHO_BOTTOM) + ORTHO_BOTTOM;
        stars[i].pos.z = (rand() / (float)RAND_MAX) * -100.0f - 5.0f; 
        
        if (rand() % 5 == 0) 
            stars[i].color = glm::vec3(1.0f, 1.0f, 0.0f);
        else 
            stars[i].color = glm::vec3(1.0f, 1.0f, 1.0f);

        stars[i].size = (rand() / (float)RAND_MAX) * 0.3f + 0.1f;
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

