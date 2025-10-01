#include <algorithm>

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

void Player::update(float deltaTime, const std::vector<Enemy*>& enemies) {
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

    for (auto& o : orbits) o.update(deltaTime);


    if (isRecovery) {
        recoveryCooldown -= deltaTime;
        if (recoveryCooldown <= 0)
            isRecovery = false; 
    }
    else {
        for (auto enemy : enemies) {
            if (!enemy || !enemy->get_isActive())
                continue;

            ObjectPool<Bullet>& pool = enemy->get_bulletPool();
            for (auto& bullet : pool.get_pool()) {
                if (!bullet->get_isActive())
                    continue;

                if (check_collision(bullet)) {
                    heart = std::max(0, heart - 1);
                    if (heart < static_cast<int>(hearts.size()))
                        hearts[heart].set_isActive(false);
                    if (heart < static_cast<int>(orbits.size()))
                        orbits[heart].set_isActive(false);
                    pool.release(bullet);
                    isRecovery = true;
                    recoveryCooldown = recoveryInterval;
                    break;
                }
            }

            if (isRecovery) break;
        }
    }

}

void Player::draw() const {
    Object::draw();
    leftCanon.draw();
    rightCanon.draw();
    for (auto& h : hearts) h.draw();
    for (auto& o : orbits) o.draw();
}

void Player::reset() {
    init(ZERO, 0, UP, glm::vec3(2,2,2));
    
    for (auto &canon : get_canons()) {
        auto &ap = canon->get_attackPool();
        for (auto &a : ap.get_pool()) 
            if(a->get_isActive())
                ap.release(a);
    }
    
    direction = ZERO;
    velocity = 15;    
    isShooting = false;
    isRecovery = false;
    isAccelerating = false;
    recoveryCooldown = 0;
    
    for (auto& h : hearts) h.set_isActive(true);
    heart = MAX_HEART;     
    for (auto& o : orbits) o.set_isActive(true);

 
    set_isActive(true);
    set_isVisible(true);
}
