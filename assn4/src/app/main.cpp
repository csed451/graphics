#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/globals/game_constants.h"
#include "core/globals/camera.h"
#include "core/base/scene_node.h"
#include "core/render/renderer.h"
#include "core/render/mesh.h"
#include "game/entities/player.h"
#include "game/entities/enemy.h"

// Constants & simple types
enum class GameState { Playing, GameOver, Exiting };
constexpr float PLAYER_INITIAL_SPEED = 0.01f;

// Global state (local TU scope)
static GameState gameState = GameState::Playing;
static GLuint starVAO = 0;
static GLuint starVBO = 0;
static GLsizei starVertexCount = 0;

static GLuint boundingBoxVAO = 0;
static GLuint boundingBoxVBO = 0;
static GLsizei boundingBoxVertexCount = 0;

static std::vector<float> starVertices;
static std::vector<Enemy*> enemies;
static Player* player = nullptr;
static int prevTime = 0;
static int windowWidth = 600;
static int windowHeight = 600;
static float playerSpeed = PLAYER_INITIAL_SPEED;
static glm::vec3 playerDirection = ZERO;
static glm::vec3 playerPrevDirection = ZERO;

// Forward declarations
static void set_projection_matrix(ProjectionType type);
static void reshape (int w, int h);
static void display (void);
static void timer(int value);
static void update(void);

static void key_down(unsigned char key, int x, int y);
static void special_key_down(int key, int x, int y);
static void key_up(unsigned char key, int x, int y);
static void special_key_up(int key, int x, int y);

static void reset_game();
static void check_and_handle_game_over();
static bool enemies_destroyed();

static void draw_ending_msg(const char* line1, const char* line2, int w, int h);
static void draw_game_over(const char* msg);

static void init_stars();
static void draw_stars();
static void init_bounding_box();
static void draw_bounding_box();

// Entry point
int main(int argc, char** argv) {
    /* initial window setup */
    glutInit (&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Bullet Hell shooter");

    // Initialize GLEW and Renderer
    glewExperimental = GL_TRUE;
    glewInit();
    gRenderer.init();

    // OpenGL states configuration
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPointSize(2.0f);

    init_camera();
    set_projection_matrix(projectionType);

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
    player->set_enemies(enemies);
    for (auto enemy : enemies) {
        enemy->set_parent(&sceneRoot);
        enemy->set_player(player);
    }

    glutMainLoop();
    
    for (auto enemy : enemies)
        delete enemy;
    delete player; 
    
    return 0;
}

// Camera / projection
static void set_projection_matrix(ProjectionType type) {
    glm::mat4 projection;
    float aspect = static_cast<float>(windowWidth) / static_cast<float>(std::max(1, windowHeight));

    projectionType = type;
    cameraTargetObject = nullptr;

    if(type == ProjectionType::Perspective) {
        projection = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 500.0f);
        init_camera();
    }
    else if(type == ProjectionType::Orthographic) {
        projection =  glm::ortho(-MAX_COORD, MAX_COORD, -MAX_COORD, MAX_COORD, -1.0f, 1.0f);
        cameraMatrix = glm::mat4(1.0f);
        gRenderer.set_view(cameraMatrix);
    }
    else if(type == ProjectionType::Thirdperson) {
        projection = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 500.0f);
        cameraTargetObject = player;
        cameraPos = glm::vec3(0, -20, 10);
    }

    gRenderer.set_projection(projection);
    update_camera();
}

// GLUT callbacks
static void reshape (int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport (0, 0, w, h);
    set_projection_matrix(projectionType);
}

static void display (void) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    update_camera();
    
    gRenderer.begin_frame();

    gRenderer.apply_render_style();
    sceneRoot.draw();
    draw_bounding_box();
    bool showOverlay = (gameState == GameState::GameOver);
    const char* overlayMsg = nullptr;
    if (showOverlay) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        draw_stars();
        overlayMsg = enemies_destroyed() ? "GAME WIN!" : "GAME OVER!";
        gRenderer.apply_render_style();
    }

    gRenderer.end_frame();

    if (showOverlay && overlayMsg)
        draw_game_over(overlayMsg);
    
    glutSwapBuffers();
}

static void timer(int /*value*/) {
    update();
    glutPostRedisplay();
    glutTimerFunc(1000.0f / FPS, timer, 0);
}

// Game loop & state
static void update(void) {
    int curTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (curTime - prevTime) / 1000.0f;
    prevTime = curTime;

    if (gameState == GameState::Exiting) {
        glutLeaveMainLoop(); return;
    }    
    if (gameState == GameState::GameOver) {
        if (player->get_pos().y <= 450) {
            playerSpeed += 0.01f;
            player->translate(UP * playerSpeed);
            player->set_isAccelerating(true);
        }   
        return;
    }
    
    sceneRoot.update(deltaTime);
    check_and_handle_game_over();
}

static void reset_game() {
    set_projection_matrix(ProjectionType::Perspective);
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

static void check_and_handle_game_over() {
    if (enemies_destroyed() || !player->get_isActive()) {
        gameState = GameState::GameOver;
        init_stars();
        player->set_isActive(true);
        player->set_isRecovery(false);
        player->set_direction(ZERO);

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
        set_projection_matrix(ProjectionType::Thirdperson);

        playerSpeed = PLAYER_INITIAL_SPEED;
    }
}

static bool enemies_destroyed() {
    return std::all_of(enemies.begin(), enemies.end(), [](const Enemy* e) {
        return e->is_destroyed();
    });
}

// Input handling
static void key_down(unsigned char key, int /*x*/, int /*y*/) {
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
                gRenderer.switch_render_style();
                break;
            case 'c':
            case 'C':
                projectionType = static_cast<ProjectionType>((static_cast<int>(projectionType) + 1) % 3);
                set_projection_matrix(projectionType);
                break;
            case 27: // ESC
                gameState = GameState::Exiting;
                break;
        }
    }
}

static void special_key_down(int key, int /*x*/, int /*y*/) {
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

static void key_up(unsigned char key, int /*x*/, int /*y*/) {
    if (gameState == GameState::GameOver)
        return;

    switch (key) {
        case ' ':
            player->set_isShooting(false);
            break;
    }
}

static void special_key_up(int key, int /*x*/, int /*y*/) {
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

// Overlay rendering helpers
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

// Starfield & bounding box helpers
static void init_stars() {
    starVertices.clear();
    srand(42);
    for (int i = 0; i < 10000; ++i) {
        starVertices.push_back(rand() % 1000 - 500);
        starVertices.push_back(rand() % 1000 - 500);
        starVertices.push_back(rand() % 1000 - 500);
    }

    // [V1] Require VAO and VBO for stars
    starVertexCount = static_cast<GLsizei>(starVertices.size() / 3);
    if (!starVAO)
        glGenVertexArrays(1, &starVAO);         // request VAO handle
    if (!starVBO)
        glGenBuffers(1, &starVBO);              // request VBO handle

    // [V2] Bind VAO and VBO 
    glBindVertexArray(starVAO);                 // make VAO current so later calls configure it
    glBindBuffer(GL_ARRAY_BUFFER, starVBO);     // bind vertex buffer for position data

    // [V3] VBO data upload
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(starVertices.size() * sizeof(float)), 
                starVertices.data(), GL_STATIC_DRAW);
    // [V4] VAO attribute setup
    glEnableVertexAttribArray(0);                                // enable attribute slot 0 (position)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr); // describe layout: 3 floats per vertex

    // [V5] Unbind to avoid accidental modification
    glBindBuffer(GL_ARRAY_BUFFER, 0);   
    glBindVertexArray(0);               
}

static void draw_stars() {
    if (!starVAO || starVertexCount == 0)
        return;

    gRenderer.draw_raw(starVAO, starVertexCount, GL_POINTS, glm::mat4(1.0f), glm::vec4(1.0f), false);

    auto sunMesh = load_mesh("assets/sphere.obj");
    if (sunMesh) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-60, 620, 20));
        model = glm::scale(model, glm::vec3(100.0f));
        gRenderer.draw_mesh(*sunMesh, model, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    }
}

static void init_bounding_box() {
    // Only create buffers once
    if (boundingBoxVAO) return;

    const float frontZ = MAX_COORD / 4.0f;
    const float backZ = -MAX_COORD / 4.0f;
    const float min = -MAX_COORD;
    const float max = MAX_COORD;

    // Bounding box lines
    std::vector<glm::vec3> vertices = {
        {min, min, frontZ}, {max, min, frontZ},
        {max, min, frontZ}, {max, max, frontZ},
        {max, max, frontZ}, {min, max, frontZ},
        {min, max, frontZ}, {min, min, frontZ},

        {min, min, backZ}, {max, min, backZ},
        {max, min, backZ}, {max, max, backZ},
        {max, max, backZ}, {min, max, backZ},
        {min, max, backZ}, {min, min, backZ},

        {min, min, frontZ}, {min, min, backZ},
        {max, min, frontZ}, {max, min, backZ},
        {max, max, frontZ}, {max, max, backZ},
        {min, max, frontZ}, {min, max, backZ}
    };

    boundingBoxVertexCount = static_cast<GLsizei>(vertices.size());

    // Generate and bind VAO and VBO
    glGenVertexArrays(1, &boundingBoxVAO);
    glGenBuffers(1, &boundingBoxVBO);
    
    // Bind VAO and VBO
    glBindVertexArray(boundingBoxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, boundingBoxVBO);

    // VBO data upload
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3)),
                 vertices.data(), GL_STATIC_DRAW);
    // VAO attribute setup
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

    // Unbind to avoid accidental modification
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void draw_bounding_box() {
    init_bounding_box();
    if (!boundingBoxVAO)
        return;

    gRenderer.draw_raw(boundingBoxVAO,
                       boundingBoxVertexCount,
                       GL_LINES,
                       glm::mat4(1.0f),
                       glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
                       false);
}
