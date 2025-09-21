#include "globals.h"
#include "enemy.h"
#include <iostream>

void Enemy::draw_shape() const {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
    glDisable(GL_CULL_FACE);

    const float outerR = 1.4f;      
    const float midR   = 0.1f;      
    const float coreR  = 0.40f;     

    // 1. outer octagon
    glColor3f(0.85f, 0.15f, 0.15f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, 0.0f);
        for (int i = 0; i <= 8; ++i) {
            float a = (float)i / 8 * TWO_PI;
            glVertex2f(std::cos(a) * outerR, std::sin(a) * outerR);
        }
    glEnd();

    // 2. middle rotated square
    glColor3f(1.0f, 0.35f, 0.15f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0,0);
        for (int i=0;i<=4;++i){
            float a = (45.0f + i*90.0f) * DEG2RAD;
            glVertex2f(std::cos(a)*midR, std::sin(a)*midR);
        }
    glEnd();

    // 3. core
    glColor3f(1.0f, 0.8f, 0.2f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0,0);
        for (int i=0;i<=24;++i){
            float a = (float)i/24 * TWO_PI;
            glVertex2f(std::cos(a)*coreR, std::sin(a)*coreR);
        }
    glEnd();

    glPopAttrib();
}

void Enemy::update(float deltaTime, Player* player) {
    // 이 여부에 따라서 Map이 멈추는 것처럼 표현 가능
    // if (!get_isActive()) return; 

    translate(glm::vec3(moveDir * moveSpeed * deltaTime, 0, 0));
    float x = get_pos().x;
    if (x > moveLimit) {
        translate(glm::vec3(moveLimit - x, 0, 0));
        moveDir = -1.0f;
    } else if (x < -moveLimit) {
        translate(glm::vec3(-moveLimit - x, 0, 0));
        moveDir = 1.0f;
    }
    
    for (auto& canon : player->get_canons()){
        ObjectPool<Attack> & pool = canon->get_attackPool();
        for (auto& attack : pool.get_pool()) {
            if (attack->get_isActive()) {
                float distance = glm::distance(this->get_pos(), attack->get_pos());
                float collision_threshold = this->get_hitboxRadius() + attack->get_hitboxRadius();

                if (distance < collision_threshold) {
                    this->take_damage(10); 
                    pool.release(attack); 
                }
            }
        }
    }    

    if(is_destroyed()){
        set_isActive(false);
        set_isVisible(false);
    }

    shootCooldown -= deltaTime;
    if(shootCooldown <= 0 && !is_destroyed()){
        shoot();
        shootCooldown = shootInterval;
    }
    bulletPool.update(deltaTime);
}

void Enemy::draw() const {
    Object::draw();
    bulletPool.draw();
    draw_health_bar();
}

void Enemy::draw_health_bar() const{
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    
    // initialize OpenGL
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); 
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); 
    glLoadIdentity();

    // Draw health bar
    float bar_width = w * 0.8f;
    float bar_height = 20;
    float start_x = (w - bar_width) / 2.0f;
    float start_y = h - bar_height - 10.0f;
    float health_ratio = (float) heart / ENEMY_MAX_HEART;

    // health bar background
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
        glVertex2f(start_x, start_y);
        glVertex2f(start_x + bar_width, start_y);
        glVertex2f(start_x + bar_width, start_y + bar_height);
        glVertex2f(start_x, start_y + bar_height);
    glEnd();

    // Remaining health bar
    glColor3f(1.0f, health_ratio, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(start_x, start_y);
        glVertex2f(start_x + bar_width * health_ratio, start_y);
        glVertex2f(start_x + bar_width * health_ratio, start_y + bar_height);
        glVertex2f(start_x, start_y + bar_height);
    glEnd();
    
    // Restore OpenGL state
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Enemy::take_damage(int damage){
    heart -= damage;
    if (heart < 0) heart = 0;
}

bool Enemy::is_destroyed() const{ 
    return heart <= 0;
}

void Enemy::shoot(){
    const int num_bullets = 20; 
    const float angle_step = 360.0f / num_bullets; 
    const float angle_offset = 9.0f;
    
    counter = !counter;
    for (int i = 0; i < num_bullets; ++i) {
        float angle = glm::radians(i * angle_step + counter * angle_offset); 
        glm::vec3 dir = glm::vec3(cos(angle), sin(angle), 0); 
        
        Bullet* bullet = bulletPool.acquire();
        bullet->init(get_pos(), 0, glm::vec3(1,0,0), glm::vec3(1,1,1));
        bullet->set_counter(counter);
        bullet->set_direction(dir); 
    }
}

void Enemy::reset(){
    init(glm::vec3(0,30,0), 0, glm::vec3(1,0,0), glm::vec3(2,2,2));
    
    for (auto &b : bulletPool.get_pool()) 
        bulletPool.release(b);

    heart = ENEMY_MAX_HEART;
    shootCooldown = shootInterval;
    counter = true;
    moveDir = 1.0f;
    set_isActive(true);
    set_isVisible(true);  
}