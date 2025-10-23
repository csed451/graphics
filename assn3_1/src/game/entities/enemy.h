#pragma once

#include <vector>
#include <algorithm>
#include <glm/gtc/constants.hpp>
#include "core/base/object.h"
#include "core/base/object_pool.h"
#include "game/ui/healthbar.h"
#include "game/weapons/bullet.h"
#include "game/attachments/upper.h"

class Player;
class Enemy : public Object{
private:
    std::vector<float> outerVertices;
    std::vector<float> innerVertices;
    ObjectPool<Bullet> bulletPool;    
    glm::vec3 spawnPosition;
    void init_vertices();

    Healthbar healthBar;
    Upper rightUpperArm;
    Upper leftUpperArm;
    
    const float shootInterval = 2.0f; 
    const float moveLimit = -15.0f;
    const float moveSpeed = 2.0f;        
    const float outerR = 1.4f;
    const float coreR  = 0.40f;
    
    float shootCooldown = shootInterval; 
    int heart = ENEMY_MAX_HEART;
    float moveDir = -1.0f;
    bool counter = true;   

public:
    Enemy(
        glm::vec3 _pos=ZERO, 
        GLfloat _angle=0, 
        glm::vec3 _axis=RIGHT, 
        glm::vec3 _size=glm::vec3(1), 
        glm::vec3 _center=ZERO
    ) : Object(_pos, _angle, _axis, _size, _center), 
        bulletPool(200),
        spawnPosition(_pos),
        healthBar(glm::vec3(0, 2.5f, 0), 0, UP, glm::vec3(1), ZERO, this),
        rightUpperArm(glm::vec3(2.0f, 0, 0), -90, FORWARD, glm::vec3(1), ZERO, this, 18.0f, 1.1f, glm::pi<float>(), 28.0f, 1.7f, glm::pi<float>()),
        leftUpperArm(glm::vec3(-2.0f, 0, 0), 90, FORWARD, glm::vec3(1), ZERO, this, 18.0f, 1.1f, 0.0f, 28.0f, 1.7f, 0.5f) {
        set_hitboxRadius(outerR);
        init_vertices();
        
        if (load_mesh("assets/drone.obj")) {
            glm::mat4 meshTransform(1.0f);
            meshTransform = glm::scale(meshTransform, glm::vec3(5.0f));
            meshTransform = glm::rotate(meshTransform, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            meshTransform = glm::rotate(meshTransform, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            set_mesh_transform(meshTransform);
            set_mesh_color(glm::vec4(0.85f, 0.15f, 0.15f, 1.0f));
        }
    };  
    
    ObjectPool<Bullet>& get_bulletPool() { return bulletPool; }

    void update(float deltaTime, Player* player);
    void draw_shape() const override;
    
    inline void take_damage(int damage) { heart = std::max(0, heart - damage); }
    inline bool is_destroyed() const { return heart <= 0; }
    int get_heart() const { return heart; }

    void shoot();
    void reset();
};
    
