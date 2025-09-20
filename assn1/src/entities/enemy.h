#pragma once

#include "object.h"
#include "objectPool.h"
#include "bullet.h"
#include "attack.h"
#include "player.h"

const int ENEMY_MAX_HEART = 100;

class Enemy : public Object{
private:
    int heart = ENEMY_MAX_HEART;
    ObjectPool<Bullet> bulletPool;

    float shootInterval = 2.0f; 
    float shootCooldown = shootInterval; 

    bool counter = true;

    float moveDir = 1.0f;          
    float moveSpeed = 4.0f;        
    float moveLimit = 30.0f;
public:
    Enemy(glm::vec3 _pos=glm::vec3(), 
            GLfloat _angle=0, 
            glm::vec3 _axis=glm::vec3(1,0,0), 
            glm::vec3 _size=glm::vec3(1), 
            glm::vec3 _center=glm::vec3()) 
        : Object(_pos, _angle, _axis, _size, _center), bulletPool(200) {};

    void draw_shape() const override;
    void update(float deltaTime, Player* player);
    void draw() const;
    
    
    void draw_health_bar() const;
    void take_damage(int damage);
    bool is_destroyed() const;
    
    ObjectPool<Bullet>& get_bulletPool() { return bulletPool; }

    void shoot();
};
    
