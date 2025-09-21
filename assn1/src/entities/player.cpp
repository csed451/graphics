#include "player.h"
#include "enemy.h"

void Player::draw_shape() const {
    glColor4f(0, 1, 0, isRecovery ? 0.2 : 1);
    glBegin(GL_TRIANGLES);
        glVertex3f(0, 1, 0);
        glVertex3f(-1, -1, 0);
        glVertex3f(1, -1, 0);
    glEnd();
    if(!isShooting && !isRecovery && direction != ZERO || isAccelerating) {
        glColor3f(1, 0, 0);
        glBegin(GL_QUADS);
            glVertex3f(-1.2, -1.8, 0);
            glVertex3f(-0.3, -1.8, 0);
            glVertex3f(-0.5, -1, 0);
            glVertex3f(-1, -1, 0);
        glEnd();
        glBegin(GL_QUADS);
            glVertex3f(0.3, -1.8, 0);
            glVertex3f(1.2, -1.8, 0);
            glVertex3f(1, -1, 0);
            glVertex3f(0.5, -1, 0);
        glEnd();
    }
}

void Player::update(float deltaTime, Enemy *enemy) {
    if (!get_isActive())
        return; 

    glm::vec3 move = velocity * direction * deltaTime;
    move *= (isShooting || isRecovery ? 0.5 : 1);
    translate(move);
    if (is_outside_window(get_pos()))
        translate(-move);

    if (heart <= 0)
        set_isActive(false);

    if (isShooting && ! isRecovery) {
        leftCanon.shoot();
        rightCanon.shoot();
    }
    leftCanon.update(deltaTime);
    rightCanon.update(deltaTime);


    if (isRecovery) {
        recoveryCooldown -= deltaTime;
        if (recoveryCooldown <= 0)
            isRecovery = false; 
    }
    else {
        ObjectPool<Bullet> & pool = enemy->get_bulletPool();
        for (auto& bullet : pool.get_pool()) {
            if (bullet->get_isActive()) {
                if (check_collision(bullet)) {
                    heart -= 1;
                    hearts[heart].set_isActive(false);
                    pool.release(bullet);
                    isRecovery = true;
                    recoveryCooldown = recoveryInterval;
                    break;
                }
            }
        }
    }

}

void Player::draw() const {
    Object::draw();
    leftCanon.draw();
    rightCanon.draw();
    for (auto& h : hearts) h.draw();
}

void Player::reset() {
    init(ZERO, 0, UP, glm::vec3(2,2,2));
    
    for (auto &canon : get_canons()) {
        auto &ap = canon->get_attackPool();
        for (auto &a : ap.get_pool()) 
            ap.release(a);
    }
    
    direction = ZERO;
    velocity = 15;    
    isShooting = false;
    isRecovery = false;
    recoveryCooldown = 0;
    
    for (auto& h : hearts) h.set_isActive(true);
    heart = MAX_HEART;     
 
    set_isActive(true);
    set_isVisible(true);
}
