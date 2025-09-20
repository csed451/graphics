#include "globals.h"
#include "enemy.h"
#include <iostream>

void Enemy::draw_shape() const {
    glColor3f(0.8, 0.1, 0.1);
    glBegin(GL_QUADS);
        glVertex3f(-1, -1, 0);
        glVertex3f(1, -1, 0);
        glVertex3f(1, 1, 0);
        glVertex3f(-1, 1, 0);
    glEnd();
}

void Enemy::update(float deltaTime, Player* player) {
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
    

    shootCooldown -= deltaTime;
    
    if(shootCooldown <= 0){
        shoot();
        shootCooldown = shootInterval;
    }
    bulletPool.update(deltaTime);

    if(is_destroyed()){
        set_isActive(false);
        set_isVisible(false);
    }
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

    for (int i = 0; i < num_bullets; ++i) {
        float angle = glm::radians(i * angle_step); 
        glm::vec3 dir = glm::vec3(cos(angle), sin(angle), 0); 
        
        Bullet* bullet = bulletPool.acquire();
        bullet->init(get_pos(), 0, glm::vec3(1,0,0), glm::vec3(1,1,1));
        bullet->set_direction(dir); 
    }
}
