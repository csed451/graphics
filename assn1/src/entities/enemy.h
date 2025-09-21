#pragma once

#include "object.h"
#include "objectPool.h"
#include "bullet.h"


const int ENEMY_MAX_HEART = 10;

class Player;
class Enemy : public Object{
private:
    ObjectPool<Bullet> bulletPool;    
    int heart = ENEMY_MAX_HEART;

    float shootInterval = 2.0f; 
    float shootCooldown = shootInterval; 

    float moveDir = 1.0f;          
    float moveSpeed = 4.0f;        
    float moveLimit = 30.0f;
    bool counter = true;
public:
    Enemy(
        glm::vec3 _pos=ZERO, 
        GLfloat _angle=0, 
        glm::vec3 _axis=RIGHT, 
        glm::vec3 _size=glm::vec3(1), 
        glm::vec3 _center=ZERO
    ) : Object(_pos, _angle, _axis, _size, _center), bulletPool(200) {
        init(_pos, _angle, _axis, _size, _center);
        set_hitboxRadius(1.4f);
    };
    
    ObjectPool<Bullet>& get_bulletPool() { return bulletPool; }

    void update(float deltaTime, Player* player);
    
    void draw_shape() const override;
    void draw() const;
    void draw_health_bar() const;
    
    inline void take_damage(int damage) { heart = std::max(0, heart - damage); }
    inline bool is_destroyed() const { return heart <= 0; }

    void shoot();
    void reset();
};
    
