#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cmath>

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
#include "app/background.h"

// Constants & simple types
enum class GameState { Playing, GameOver, Exiting };
static bool gPaused = false;
constexpr float PLAYER_INITIAL_SPEED = 0.01f;
constexpr float PLAYER_LIGHT_RADIUS = 10.0f;
constexpr float PLAYER_LIGHT_SPEED = 2.5f; // rad/sec
constexpr float PLAYER_LIGHT_HEIGHT = 0.0f;
constexpr float ENEMY_LIGHT_HEIGHT = 5.0f;
// for shadow map
const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;
static bool gShadowOn = false;

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
static DirectionalLight dirLight;
static PointLight playerLight;
static PointLight enemyLight;
static float playerLightAngle = 0.0f;
static bool gDayMode = true; // toggle manually for night build

// for shadow map
GLuint depthMapFBO = 0;
GLuint depthMapTexture = 0;
// for scene map
GLuint sceneFBO = 0;
GLuint colorTexture = 0;
GLuint velocityTexture = 0;

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

static void init_shadow_map();
static void init_scene_map();

// 전역 변수나 정적 변수로 VAO, VBO 핸들을 저장해둡니다.
static GLuint quadVAO = 0;
static GLuint quadVBO = 0;

void draw_screen_quad() {
    // 1. 만약 VAO가 0이라면(아직 안 만들어졌다면) 생성합니다. (최초 1회 실행)
    if (quadVAO == 0) {
        float quadVertices[] = {
            // 위치(Pos)       // 텍스처 좌표(TexCoords)
            // x,     y,       u,    v
            -1.0f,  1.0f,     0.0f, 1.0f, // 왼쪽 위
            -1.0f, -1.0f,     0.0f, 0.0f, // 왼쪽 아래
             1.0f, -1.0f,     1.0f, 0.0f, // 오른쪽 아래

            -1.0f,  1.0f,     0.0f, 1.0f, // 왼쪽 위
             1.0f, -1.0f,     1.0f, 0.0f, // 오른쪽 아래
             1.0f,  1.0f,     1.0f, 1.0f  // 오른쪽 위
        };

        // VAO, VBO 생성
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);

        glBindVertexArray(quadVAO);

        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        // 속성 0: 위치 (vec2)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // 속성 1: 텍스처 좌표 (vec2)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        
        // 바인딩 해제 (안전장치)
        glBindVertexArray(0);
    }

    // 2. VAO 바인딩 후 그리기
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6); // 정점 6개로 삼각형 2개 그리기
    glBindVertexArray(0);
}

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
    // init shadow map (init FBO and depth texture)
    init_shadow_map();
    init_scene_map();
    gRenderer.init();

    const GLubyte* renderer = glGetString(GL_RENDERER);
    if (renderer) {
        std::cout << "🖥️ Renderer: " << renderer << std::endl;
    }

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
    background::init(MAX_COORD, gDayMode);

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

    // initial lights
    dirLight.direction = glm::normalize(glm::vec3(0.0f, 0.1f, -0.4f));
    dirLight.color = glm::vec3(1.0f);
    dirLight.intensity = 1.5f;
    // Player-following point light (orbits around player)
    playerLight.position = player->get_pos() + glm::vec3(PLAYER_LIGHT_RADIUS, 0.0f, PLAYER_LIGHT_HEIGHT);
    playerLight.color = glm::vec3(1.0f);
    playerLight.intensity = 0.0f;

    // Enemy accent light
    enemyLight.color = glm::vec3(0.5f, 0.5f, 1.0f);
    enemyLight.intensity = 0.0f;
    if (!enemies.empty())
        enemyLight.position = enemies.front()->get_pos() + glm::vec3(0.0f, 0.0f, ENEMY_LIGHT_HEIGHT);

    std::vector<PointLight> initialLights;
    initialLights.reserve(MAX_POINT_LIGHTS);
    initialLights.push_back(playerLight); // slot 0 = player light
    for (auto enemy : enemies) {
        if (!enemy)
            continue;
        enemyLight.position = enemy->get_pos() + glm::vec3(0.0f, 0.0f, ENEMY_LIGHT_HEIGHT);
        initialLights.push_back(enemyLight); // copy with per-enemy position
        if (static_cast<int>(initialLights.size()) >= MAX_POINT_LIGHTS)
            break;
    }
    gRenderer.set_lights(dirLight, initialLights);
    gRenderer.set_view_position(cameraPos);


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
    else if(type == ProjectionType::Thirdperson) {
        projection = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 500.0f);
        cameraTargetObject = player;
        // cameraPos = glm::vec3(0, -100, 50);
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
    glm::vec3 eye = cameraTargetObject ? (cameraTargetObject->get_pos() + cameraPos) : cameraPos;
    gRenderer.set_view_position(eye); 

    // 1. Depth pass (generate shadow map)
    ShadingMode prevShading = gRenderer.get_shading_mode();
    gRenderer.set_shading_mode(ShadingMode::DepthOnly);
    gRenderer.set_light_space_matrix();

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    // glCullFace(GL_FRONT);

    sceneRoot.draw();

    // glCullFace(GL_BACK);
    
    // 2. Lighting pass (regular rendering with shadows)
    gRenderer.set_shading_mode(prevShading);
    glViewport(0, 0, windowWidth, windowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
    GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // if (gShadowOn)
    //     gRenderer.set_shadow_map(depthMapTexture);
    // else
    //     gRenderer.set_shadow_map(0);

    background::draw();
    sceneRoot.draw();
    draw_bounding_box();
    
    // 3. Blur pass
    gRenderer.set_shading_mode(ShadingMode::MotionBlur);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT); // 깊이는 필요 없음 (Quad만 그릴 거라)

    // 아까 2번 단계에서 FBO에 저장해둔 텍스처들을 가져옴
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexture); // 2번의 결과물 (색상)
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, velocityTexture); // 2번의 결과물 (속도)

    // 화면 전체를 덮는 사각형(Quad) 하나만 딱 그림
    draw_screen_quad();
    glEnable(GL_DEPTH_TEST);

    gRenderer.set_shading_mode(prevShading);
    
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

    if (gPaused)
        return;

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
    
    // animate player-follow point light (orbit)
    playerLightAngle += deltaTime * PLAYER_LIGHT_SPEED;
    playerLight.position = player->get_pos() + glm::vec3(std::cos(playerLightAngle) * PLAYER_LIGHT_RADIUS,
                                                         std::sin(playerLightAngle) * PLAYER_LIGHT_RADIUS,
                                                         PLAYER_LIGHT_HEIGHT);

    std::vector<PointLight> pointLights;
    pointLights.reserve(MAX_POINT_LIGHTS);
    pointLights.push_back(playerLight); // slot 0 = orbiting player light

    for (auto enemy : enemies) {
        if (!(enemy && enemy->get_isActive() && !enemy->is_destroyed()))
            continue;
        enemyLight.position = enemy->get_pos() + glm::vec3(0.0f, 0.0f, ENEMY_LIGHT_HEIGHT);
        pointLights.push_back(enemyLight); // one light per active enemy (up to MAX_POINT_LIGHTS)
        if (static_cast<int>(pointLights.size()) >= MAX_POINT_LIGHTS)
            break;
    }

    gRenderer.set_lights(dirLight, pointLights);

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
            case 't':
            case 'T':
                gPaused = !gPaused;
                break;
            case 'b':
            case 'B':
                gDayMode = !gDayMode;
                background::set_day_mode(gDayMode);
                break;
            case 'w':
            case 'W':
                gRenderer.switch_shading_mode();
                break;
            case 'c':
            case 'C':
                // Cycle only between Perspective (top view) and Thirdperson (close view) per assn4 spec
                projectionType = (projectionType == ProjectionType::Perspective) ? ProjectionType::Thirdperson : ProjectionType::Perspective;
                set_projection_matrix(projectionType);
                break;
            case 's':
            case 'S':
                gShadowOn = !gShadowOn;
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

    auto sunMesh = load_mesh("assets/models/sphere.obj");
    if (sunMesh) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-60, 620, 20));
        model = glm::scale(model, glm::vec3(100.0f));
        gRenderer.draw_mesh(*sunMesh, model, model, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
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

static void init_shadow_map() {
    // Generate Framebuffer Object (FBO)
    glGenFramebuffers(1, &depthMapFBO);
    
    // Generate depth texture (Shadow map)
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    
    // Configure texture to store only depth components
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    
    // Set texture filtering parameters (Mipmaps are not needed)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Set texture wrapping parameters (Areas outside the shadow map are set to 1.0 to prevent shadow projection)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    // Attach the depth texture to the FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
    
    // Disable writes to the color buffer (We only need depth data)
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    // Unbind the framebuffer (Switch back to default)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


static void init_scene_map() {
    // 1. FBO 생성
    glGenFramebuffers(1, &sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
    
    // 2. Color Texture 생성 (일반 화면용: RGB)
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    
    // [필수] 필터링 설정 (이게 없으면 텍스처가 검게 나옴)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // FBO에 부착 (0번 슬롯)
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    
    // 3. Velocity Texture 생성 (속도 저장용: RG16F - 정밀도 필요)
    glGenTextures(1, &velocityTexture);
    glBindTexture(GL_TEXTURE_2D, velocityTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);

    
    // [필수] 필터링 설정 (속도 정보는 보간되면 안 되므로 NEAREST 추천, 하지만 부드러움을 위해 LINEAR도 가능)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // FBO에 부착 (1번 슬롯)
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, velocityTexture, 0);

    // 4. 깊이 버퍼 (RBO) 생성
    // FBO에 깊이 버퍼가 없으면 Depth Test가 작동하지 않아 물체 순서가 엉망이 됨
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // 깊이 컴포넌트 저장소 할당
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
    // FBO에 부착
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // 5. Draw Buffers 설정 (MRT 핵심)
    GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    // 6. 상태 확인
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER:: Scene Framebuffer is not complete!" << std::endl;
    }

    // 설정 끝났으니 기본 프레임버퍼로 복귀
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}