#include <iostream>

#include "globals.h"
#include "enemy.h"
#include "player.h"

void Enemy::init_vertices() {
    // outer octagon Vertices
    outerVertices.push_back(0.0f); 
    outerVertices.push_back(0.0f);
    for (int i = 0; i <= 8; ++i) {
        float a = (float)i / 8 * TWO_PI;
        outerVertices.push_back(std::cos(a) * outerR);
        outerVertices.push_back(std::sin(a) * outerR);
    }

    // core Vertices
    innerVertices.push_back(0.0f); 
    innerVertices.push_back(0.0f);
    for (int i = 0; i <= 24; ++i){
        float a = (float)i/24 * TWO_PI;
        innerVertices.push_back(std::cos(a)*coreR);
        innerVertices.push_back(std::sin(a)*coreR);
    }
}

void Enemy::update(float deltaTime, Player* player) {
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
                    this->take_damage(attack->get_damage());
                    pool.release(attack); 
                }
            }
        }
    }    

    if(is_destroyed()){
        set_isActive(false);
        set_isVisible(false);
        for (auto &b : bulletPool.get_pool()) 
            b->set_isVisible(false);
    }

    shootCooldown -= deltaTime;
    if(shootCooldown <= 0 && !is_destroyed()){
        shoot();
        shootCooldown = shootInterval;
    }
    bulletPool.update(deltaTime);
}

void Enemy::draw_shape() const {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_CULL_FACE);
    glEnableClientState(GL_VERTEX_ARRAY);

    // outer octagon
    glColor3f(0.85f, 0.15f, 0.15f);
    glVertexPointer(2, GL_FLOAT, 0, outerVertices.data());
    glDrawArrays(GL_TRIANGLE_FAN, 0, outerVertices.size() / 2);

    // core Vertices
    glColor3f(1.0f, 0.8f, 0.2f);
    glVertexPointer(2, GL_FLOAT, 0, innerVertices.data());
    glDrawArrays(GL_TRIANGLE_FAN, 0, innerVertices.size() / 2);

    glDisableClientState(GL_VERTEX_ARRAY);
    glPopAttrib();
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
    moveDir = 1.0f;
    counter = true;
    set_isActive(true);
    set_isVisible(true);  
}