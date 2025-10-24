#include <algorithm>

#include "player.h"
#include "game/entities/enemy.h"
#include "core/globals/camera.h"

void Player::draw_shape() const {
    glColor4f(0, 1, 0, isRecovery ? 0.2f : 1.0f);
    GLfloat scaleFactor = 0.3f;
    
    glPushMatrix();
    glScalef(scaleFactor, scaleFactor, scaleFactor);    
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
    get_mesh()->draw();
    glPopMatrix();
}

void Player::draw() const {
    if (get_isActive() && get_isVisible()) {
        glPushMatrix();
        glm::mat4 finalMatrix = get_finalMatrix();
        glm::mat4 mvp = cameraMatrix * finalMatrix;
        glLoadMatrixf(glm::value_ptr(mvp));

        if (direction == UP)
            glRotatef(-20.0f, 1.0f, 0.0f, 0.0f);
        else if (direction == DOWN)
            glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
        else if (direction == RIGHT)
            glRotatef(20.0f, 0.0f, 1.0f, 0.0f);
        else if (direction == LEFT)
            glRotatef(-20.0f, 0.0f, 1.0f, 0.0f);     

        draw_shape();

        for (auto child : get_children())
            if (child)
                child->draw();

        glPopMatrix();
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
    
    heart = MAX_HEART;
    int i = 1;
    for (auto& o : orbits) {
        o.init(glm::vec3(5,0,0), 0, UP, glm::vec3(2));
        o.rotate_world(360.0f/heart * (i-1), FORWARD);
        o.set_isActive(true);
        i++;
    }

 
    set_isActive(true);
    set_isVisible(true);
}
