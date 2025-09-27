#pragma once

#include <vector>

#include "object.h"
#include "canon.h"
#include "heart.h"
#include "orbit.h"

class Enemy;

constexpr int MAX_HEART = 3;

class Player : public Object {
private:
    Canon leftCanon;
    Canon rightCanon;
    
    glm::vec3 direction = ZERO;
    float velocity = 15;
    
    bool isShooting = false;
    bool isRecovery = false;
    bool isAccelerating = false;

    const float recoveryInterval = 3;
    float recoveryCooldown = 0;

    std::vector<Heart> hearts;
    int heart = MAX_HEART;

    std::vector<Orbit> orbits;
public:
    Player(
        glm::vec3 _pos=ZERO, 
        GLfloat _angle=0,
        glm::vec3 _axis=UP, 
        glm::vec3 _size=glm::vec3(1), 
        glm::vec3 _center=ZERO
    ) : Object(_pos, _angle, _axis, _size, _center), leftCanon(), rightCanon() {
        leftCanon.init(glm::vec3(-0.8, 0.2, 0));
        rightCanon.init(glm::vec3(0.8, 0.2, 0));
        leftCanon.set_parent(this);
        rightCanon.set_parent(this);
        for (int i = 1; i <= heart; i++) {
            hearts.emplace_back().init(glm::vec3(-MAX_COORD + i * 5, -MAX_COORD + 5, 0), 0, UP, glm::vec3(2));
            Orbit & orbit = orbits.emplace_back();
            orbit.init(glm::vec3(5,0,0), 0, UP, glm::vec3(2));
            orbit.rotate_world(360.0f/heart * (i-1), FORWARD);
            orbit.set_parent(this);
        }

    };

    std::vector<Canon*> get_canons() { return {&leftCanon, &rightCanon}; }
    glm::vec3 get_direction() { return direction; }
    
    void set_velocity(float v) { velocity = v; }
    void set_direction(glm::vec3 v) { direction = v; }
    void set_isShooting(bool b) { isShooting = b; }
    void set_isRecovery(bool b) { isRecovery = b; }
    void set_isAccelerating(bool b) { isAccelerating = b; }

    void update(float deltaTime, Enemy *enemy);

    void draw() const;
    void draw_shape() const override;

    void reset();
};