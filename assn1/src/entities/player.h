#pragma once

#include "object.h"
#include "canon.h"
#include <vector>

class Player : public Object {
private:
    Canon leftCanon;
    Canon rightCanon;
    
    float velocity = 15;
    glm::vec3 direction = ZERO;
    
    bool isShooting = false;
    int heart = 10;
public:
    Player(glm::vec3 _pos=ZERO, 
           GLfloat _angle=0,
           glm::vec3 _axis=UP, 
           glm::vec3 _size=glm::vec3(1), 
           glm::vec3 _center=ZERO) 
        : Object(_pos, _angle, _axis, _size, _center), leftCanon(), rightCanon() {
        leftCanon.init(glm::vec3(-0.8, 0.2, 0));
        rightCanon.init(glm::vec3(0.8, 0.2, 0));
        leftCanon.set_parent(this);
        rightCanon.set_parent(this);
    };

    void draw_shape() const override;
    void update(float deltaTime);
    void draw() const;
    void reset();
    
    std::vector<Canon*> get_canons() { return {&leftCanon, &rightCanon}; }
    glm::vec3 get_direction() { return direction; }
    
    void set_velocity(float v) { velocity = v; }
    void set_direction(glm::vec3 v) { direction = v; }
    void set_isShooting(bool b) { isShooting = b; }
    
};