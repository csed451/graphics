#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/globals/game_constants.h"
#include "core/globals/camera.h"
#include "core/base/scene_node.h"
// #include "core/base/scene_context.h"
#include "game/entities/player.h"
#include "game/entities/enemy.h"


enum class GameState { Playing, GameOver, Exiting };
GameState gameState = GameState::Playing;

enum class RenderStyle { Opaque, Wireframe };
RenderStyle currentStyle = RenderStyle::Opaque;

// SceneNode sceneRoot;

void apply_render_style(RenderStyle style) {
    switch (style) {
    case RenderStyle::Opaque:                        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_POLYGON_OFFSET_LINE);
        glLineWidth(1.0f);
        break;
    case RenderStyle::Wireframe:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1.0f, -1.0f);
        glLineWidth(1.2f);
        break;
    }
}

std::vector<float> starVertices;
std::vector<Enemy*> enemies;
Player* player = nullptr;
int prevTime = 0;

constexpr float PLAYER_INITIAL_SPEED = 0.01f;
constexpr float CAMERA_INITIAL_SPEED = 0.05f;

float playerSpeed = PLAYER_INITIAL_SPEED;
float cameraSpeed = CAMERA_INITIAL_SPEED;

inline glm::vec3 playerDirection = ZERO;
inline glm::vec3 playerPrevDirection = ZERO;


void reshape (int w, int h);
void display (void);
void timer(int value);
void update(void);

void key_down(unsigned char key, int x, int y);
void special_key_down(int key, int x, int y);
void key_up(unsigned char key, int x, int y);
void special_key_up(int key, int x, int y);

void reset_game();

static void update_game_over();
static void check_and_handle_game_over();
static bool enemies_destroyed();

static void draw_ending_msg(const char* line1, const char* line2, int w, int h);
static void draw_game_over(const char* msg);

static void init_stars();
static void draw_stars();


int main(int argc, char** argv) {
    /* initial window setup */
    glutInit (&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Bullet Hell shooter");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable depth buffer to prepare for 3D rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    init_camera();

    /* connect call back function */
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(key_down);
    glutSpecialFunc(special_key_down); 
    glutKeyboardUpFunc(key_up);
    glutSpecialUpFunc(special_key_up);
    glutTimerFunc(0, timer, 0);

    prevTime = glutGet(GLUT_ELAPSED_TIME);

    enemies.push_back(new Enemy(glm::vec3(-20, 50, 0), 0, DOWN, glm::vec3(2)));
    enemies.push_back(new Enemy(glm::vec3(20, 30, 0), 0, DOWN, glm::vec3(2)));
    player = new Player(glm::vec3(0,0,0), 0, UP, glm::vec3(2));

    player->set_parent(&sceneRoot);
    for (auto enemy : enemies)
        enemy->set_parent(&sceneRoot);



    glutMainLoop();
    
    for (auto enemy : enemies)
        delete enemy;
    delete player; 
    
    return 0;
}

void reshape (int w, int h) {
    glViewport (0, 0, w, h);    
    float aspect = (h == 0) ? 1.0f : static_cast<float>(w) / static_cast<float>(h);
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 500.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(glm::value_ptr(projection));
    update_camera();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display (void) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    apply_render_style(currentStyle);
    sceneRoot.draw();

    if (gameState == GameState::GameOver) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        draw_stars();
        const char* msg = enemies_destroyed() ? "GAME WIN!" : "GAME OVER!";
        draw_game_over(msg);
        apply_render_style(currentStyle);
    }
    
    glutSwapBuffers();
}

void timer(int /*value*/) {
    update();
    glutPostRedisplay();
    glutTimerFunc(1000.0f / FPS, timer, 0);
}

void update(void) {
    int curTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (curTime - prevTime) / 1000.0f;
    prevTime = curTime;

    if (gameState == GameState::Exiting) {
        glutLeaveMainLoop(); return;
    }    
    if (gameState == GameState::GameOver) {
        update_game_over(); return;
    }
    
    for (auto enemy : enemies)
        enemy->update(deltaTime, player);
    player->update(deltaTime, enemies);
    
    check_and_handle_game_over();
}

void key_down(unsigned char key, int /*x*/, int /*y*/) {
    if (gameState == GameState::GameOver) {
        if (key == 'r' || key == 'R') 
            reset_game();
        else if (key == 'q' || key == 'Q' || key == 27) 
            gameState = GameState::Exiting;
        return;
    }
    else {
        switch (key) {
            case ' ': 
                player->set_isShooting(true); 
                break;
            case 'w':
            case 'W':
                currentStyle = (currentStyle == RenderStyle::Opaque)
                    ? RenderStyle::Wireframe
                    : RenderStyle::Opaque;
                break;
            case 27: // ESC
                gameState = GameState::Exiting;
                break;
        }
    }
}

void special_key_down(int key, int /*x*/, int /*y*/) {
    if (gameState == GameState::GameOver)
        return;

    switch (key) {
        case GLUT_KEY_UP:
            player->set_direction(UP);
            playerPrevDirection = playerDirection;
            playerDirection = UP;
            break;
        case GLUT_KEY_DOWN:
            player->set_direction(DOWN);
            playerPrevDirection = playerDirection;
            playerDirection = DOWN;
            break;
        case GLUT_KEY_LEFT:
            player->set_direction(LEFT);
            playerPrevDirection = playerDirection;
            playerDirection = LEFT;
            break;
        case GLUT_KEY_RIGHT:
            player->set_direction(RIGHT);
            playerPrevDirection = playerDirection;
            playerDirection = RIGHT;
            break;
    }
}

void key_up(unsigned char key, int /*x*/, int /*y*/) {
    if (gameState == GameState::GameOver)
        return;

    switch (key) {
        case ' ':
            player->set_isShooting(false);
            break;
    }
}

void special_key_up(int key, int /*x*/, int /*y*/) {
    if (gameState == GameState::GameOver)
        return;

    glm::vec3 direction = player->get_direction();
    switch (key) {
        case GLUT_KEY_UP:
            if (playerDirection == UP) {
                player->set_direction(playerPrevDirection);
                playerDirection = playerPrevDirection;
                playerPrevDirection = ZERO;
            }
            else if (playerPrevDirection == UP)
                playerPrevDirection = ZERO;
            break;
        case GLUT_KEY_DOWN:
            if (direction == DOWN) {
                player->set_direction(playerPrevDirection);
                playerDirection = playerPrevDirection;
                playerPrevDirection = ZERO;
            }
            else if (playerPrevDirection == DOWN)
                playerPrevDirection = ZERO;
            break;
        case GLUT_KEY_LEFT:
            if (direction == LEFT) {
                player->set_direction(playerPrevDirection);
                playerDirection = playerPrevDirection;
                playerPrevDirection = ZERO;
            }
            else if (playerPrevDirection == LEFT)
                playerPrevDirection = ZERO;
            break;
        case GLUT_KEY_RIGHT:
            if (direction == RIGHT) {
                player->set_direction(playerPrevDirection);
                playerDirection = playerPrevDirection;
                playerPrevDirection = ZERO;
            }
            else if (playerPrevDirection == RIGHT)
                playerPrevDirection = ZERO;
            break;
    }
}

void reset_game() {
    init_camera();
    prevTime = glutGet(GLUT_ELAPSED_TIME);
    gameState = GameState::Playing;
    playerDirection = ZERO;
    playerPrevDirection = ZERO;
    starVertices.clear();
    for (auto enemy : enemies)
        enemy->reset();
    player->reset();
    glutPostRedisplay();
}

static void update_game_over() {
    if (cameraPos.z > 10) {
        cameraPos.z -= (100 - 10)/100.0f;
        cameraPos.y -= (10)/100.0f;
    }
    else if (cameraPos.y <= 450) {
        cameraPos.y += cameraSpeed;
        cameraTarget = player->get_pos();
        player->translate(UP * playerSpeed);
        playerSpeed += 0.0001;
        if (playerSpeed > 0.05) {
            playerSpeed += 0.001;
            cameraSpeed += 0.001;
            player->set_isAccelerating(true);
        }
    }
    update_camera();
}

static void check_and_handle_game_over() {
    if (enemies_destroyed() || !player->get_isActive()) {
        gameState = GameState::GameOver;
        init_stars();
        player->set_isActive(true);
        player->set_isRecovery(false);

        for (auto& canon : player->get_canons()){
            ObjectPool<Attack> & pool = canon->get_attackPool();
            for (auto& attack : pool.get_pool())
                if (attack->get_isActive())
                    pool.release(attack); 
        }

        for (auto enemy : enemies) {
            ObjectPool<Bullet>& pool = enemy->get_bulletPool();
            for (auto bullet : pool.get_pool())
                if (bullet->get_isActive())
                    pool.release(bullet);
        }

        /* set camera pos for animation */
        glm::vec3 pos = player->get_pos();
        cameraTarget.x = pos.x;
        cameraTarget.y = pos.y + 5;
        cameraPos.x = pos.x;
        cameraPos.y = pos.y + 5;
        update_camera();

        playerSpeed = PLAYER_INITIAL_SPEED;
        cameraSpeed = CAMERA_INITIAL_SPEED;
    }
}

static bool enemies_destroyed() {
    return std::all_of(enemies.begin(), enemies.end(), [](const Enemy* e) {
        return e->is_destroyed();
    });
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

    float line1Y = h * 0.8f + (fontH * 0.5f + gap * 0.5f);
    float line2Y = h * 0.8f - (fontH * 0.5f + gap * 0.5f);
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

    // // Translucent gray background
    // glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
    // glBegin(GL_QUADS);
    //     glVertex2f(0,0);
    //     glVertex2f(w,0);
    //     glVertex2f(w,h);
    //     glVertex2f(0,h);
    // glEnd();

    draw_ending_msg(msg, "Press R to Restart / Q to Quit", w, h);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

static void init_stars() {
    starVertices.clear();
    srand(42);
    for (int i = 0; i < 100000; ++i) {
        starVertices.push_back(rand() % 1000 - 500);
        starVertices.push_back(rand() % 1000 - 500);
        starVertices.push_back(rand() % 1000 - 500);
    }
}

static void draw_stars() {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixf(glm::value_ptr(cameraMatrix));

    glColor3f(1.0f, 1.0f, 1.0f);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, starVertices.data());
    glDrawArrays(GL_POINTS, 0, starVertices.size() / 3);
    glDisableClientState(GL_VERTEX_ARRAY);

    // glBegin(GL_POINTS);
    // for (size_t i = 0; i < starVertices.size(); i += 3) {
    //     float x = starVertices[i];
    //     float y = starVertices[i + 1];
    //     float z = starVertices[i + 2];
    //     glVertex3f(x, y, z);
    // }
    // glEnd();

    glTranslatef(-60, 620, 20);
    glColor3f(1,0,0);
    glutSolidSphere(100, 64, 64);

    glPopMatrix();
}
