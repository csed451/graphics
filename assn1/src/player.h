#pragma once

#include "object.h"
#include "objectPool.h"
#include "attack.h"
#include "canon.h"

class Player : public Object {
private:
    Canon leftCanon;
    Canon rightCanon;
    ObjectPool<Attack> attackPool;
    float velocity = 15;
    glm::vec3 direction = glm::vec3(0, 0, 0);
    float rps = 5;
    float lastShootTime = glutGet(GLUT_ELAPSED_TIME);

    int heart = 10;
public:
    Player(glm::vec3 _pos=glm::vec3(), GLfloat _angle=0, glm::vec3 _axis=glm::vec3(1,0,0), glm::vec3 _size=glm::vec3(1), glm::vec3 _center=glm::vec3()) : Object(_pos, _angle, _axis, _size, _center), attackPool(50), leftCanon(), rightCanon() {
        leftCanon.init(glm::vec3(-1, 0.5, 0));
        rightCanon.init(glm::vec3(1, 0.5, 0));
        leftCanon.set_parent(this);
        rightCanon.set_parent(this);
    };

    void draw_shape() const override;
    void update(float deltaTime);
    void draw() const;

    void set_velocity(float v) { velocity = v; }
    void set_direction(glm::vec3 v) { direction = v; }
    void set_rps(float r) { rps = r; }

    ObjectPool<Attack>& get_attackPool() { return attackPool; }

    void shoot();
};