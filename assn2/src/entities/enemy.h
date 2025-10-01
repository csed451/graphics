#pragma once

#include <vector>
#include <glm/gtc/constants.hpp>
#include "object.h"
#include "objectPool.h"
#include "bullet.h"
#include "upper.h"


const int ENEMY_MAX_HEART = 10;

class Player;
class Enemy : public Object{
private:
    std::vector<float> outerVertices;
    std::vector<float> innerVertices;
    void init_vertices();

    ObjectPool<Bullet> bulletPool;    
    int heart = ENEMY_MAX_HEART;

    Upper leftUpperArm;
    Upper rightUpperArm;

    float animationTime = 0.0f;

    const float outerR = 1.4f;
    const float coreR  = 0.40f;
    
    const float shootInterval = 2.0f; 
    float shootCooldown = shootInterval; 

    const float moveLimit = -15.0f;
    const float moveSpeed = 2.0f;        
    float moveDir = -1.0f;          
    bool counter = true;

    glm::vec3 spawnPosition;
    float horizontalPhase;
    float horizontalAmplitude = 4.0f;
public:
    Enemy(
        glm::vec3 _pos=ZERO, 
        GLfloat _angle=0, 
        glm::vec3 _axis=RIGHT, 
        glm::vec3 _size=glm::vec3(1), 
        glm::vec3 _center=ZERO
    ) : Object(_pos, _angle, _axis, _size, _center), 
        leftUpperArm(glm::vec3(-2.0f, 0, 0), 90, FORWARD, glm::vec3(1), ZERO, this, 
                    18.0f, 1.1f, 0.0f, 28.0f, 1.7f, 0.5f),
        rightUpperArm(glm::vec3(2.0f, 0, 0), -90, FORWARD, glm::vec3(1), ZERO, this, 
                    18.0f, 1.1f, glm::pi<float>(), 28.0f, 1.7f, glm::pi<float>()),
        bulletPool(200),
        spawnPosition(_pos),
        horizontalPhase(glm::abs(_pos.x) * 0.1f + _pos.y * 0.05f) {
        set_hitboxRadius(outerR);
        init_vertices();        
    };  
    
    ObjectPool<Bullet>& get_bulletPool() { return bulletPool; }

    void update(float deltaTime, Player* player);
    void draw_shape() const override;
    void draw() const;
    
    inline void take_damage(int damage) { heart = std::max(0, heart - damage); }
    inline bool is_destroyed() const { return heart <= 0; }
    void draw_health_bar() const;    

    void shoot();
    void reset();
};
    
