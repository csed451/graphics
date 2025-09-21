#pragma once

#include <vector>
#include "object.h"
#include "objectPool.h"
#include "bullet.h"


const int ENEMY_MAX_HEART = 10;

class Player;
class Enemy : public Object{
private:
    std::vector<float> outerVertices;
    std::vector<float> innerVertices;
    void init_vertices();

    ObjectPool<Bullet> bulletPool;    
    int heart = ENEMY_MAX_HEART;

    const float outerR = 1.4f;
    const float coreR  = 0.40f;
    
    const float shootInterval = 2.0f; 
    float shootCooldown = shootInterval; 

    const float moveLimit = 30.0f;
    const float moveSpeed = 4.0f;        
    float moveDir = 1.0f;          
    bool counter = true;
public:
    Enemy(
        glm::vec3 _pos=ZERO, 
        GLfloat _angle=0, 
        glm::vec3 _axis=RIGHT, 
        glm::vec3 _size=glm::vec3(1), 
        glm::vec3 _center=ZERO
    ) : Object(_pos, _angle, _axis, _size, _center), bulletPool(200) {
        set_hitboxRadius(outerR);
        init_vertices();
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
    
