#pragma once

#include "object.h"
#include "canon.h"
#include "bullet.h"
#include "heart.h"
#include <vector>

class Player : public Object {
private:
    Canon leftCanon;
    Canon rightCanon;
    
    float velocity = 15;
    glm::vec3 direction = ZERO;
    
    bool isShooting = false;
    bool isRecovery = false;

    float recoveryInterval = 3;
    float recoveryCooldown = 0;

    int heart = 10;
    std::vector<Heart> hearts;
public:
    Player(glm::vec3 _pos=ZERO, 
           GLfloat _angle=0,
           glm::vec3 _axis=UP, 
           glm::vec3 _size=glm::vec3(1), 
           glm::vec3 _center=ZERO) 
        : Object(_pos, _angle, _axis, _size, _center), leftCanon(), rightCanon() {
        init(_pos, _angle, _axis, _size, _center);
        leftCanon.init(glm::vec3(-0.8, 0.2, 0));
        rightCanon.init(glm::vec3(0.8, 0.2, 0));
        leftCanon.set_parent(this);
        rightCanon.set_parent(this);
        for (int i = 1; i <= heart; i++) {
            hearts.emplace_back().init(glm::vec3(ORTHO_LEFT + i * 5, ORTHO_BOTTOM + 5, 0), 0, UP, glm::vec3(2));
        }
    };

    void draw_shape() const override;
    void update(float deltaTime, ObjectPool<Bullet>& pool);
    void draw() const;
    void reset();
    
    std::vector<Canon*> get_canons() { return {&leftCanon, &rightCanon}; }
    glm::vec3 get_direction() { return direction; }
    
    void set_velocity(float v) { velocity = v; }
    void set_direction(glm::vec3 v) { direction = v; }
    void set_isShooting(bool b) { isShooting = b; }
    void set_isRecovery(bool b) { isRecovery = b; }

};