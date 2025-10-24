#include <iostream>
#include <cmath>

#include "core/globals/game_constants.h"
#include "enemy.h"
#include "game/entities/player.h"

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
    if (!get_isActive())
        return;
    
    translate(glm::vec3(0, moveDir * moveSpeed * deltaTime, 0));

    float y = get_pos().y;
    if (MAX_COORD < y) {
        translate(glm::vec3(0, MAX_COORD - y, 0));
        moveDir = -1.0f;
    } else if (y < moveLimit) {
        translate(glm::vec3(0, moveLimit - y, 0));
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
        healthBar.deactivate();
        leftUpperArm.deactivate();
        rightUpperArm.deactivate();
        for (auto &b : bulletPool.get_pool()) 
            if(b->get_isActive())
                b->set_isActive(false);
    }

    shootCooldown -= deltaTime;
    if(shootCooldown <= 0 && !is_destroyed()){
        shoot();
        shootCooldown = shootInterval;
    }
    bulletPool.update(deltaTime);

    leftUpperArm.update(deltaTime);
    rightUpperArm.update(deltaTime);
}

void Enemy::draw_shape() const {
    glColor4f(0.85f, 0.15f, 0.15f, 1.0f);
    GLfloat scaleFactor = 5.0f;
    
    glPushMatrix();
    glScalef(scaleFactor, scaleFactor, scaleFactor);    
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
    get_mesh()->draw();
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
        bullet->init(get_pos(), 0, RIGHT, glm::vec3(1));
        bullet->set_counter(counter);
        bullet->set_direction(dir); 
    }
}

void Enemy::reset(){
    init(spawnPosition, 0, RIGHT, glm::vec3(2));
    set_isActive(true);
    set_isVisible(true);  

    for (auto &b : bulletPool.get_pool()) 
        if(b->get_isActive())
            bulletPool.release(b);

    shootCooldown = shootInterval;
    heart = ENEMY_MAX_HEART;
    moveDir = -1.0f;
    counter = true;
    
    leftUpperArm.reset();
    rightUpperArm.reset();
    healthBar.reset();
}
