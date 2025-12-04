#include <algorithm>
#include <iostream>

#include "player.h"
#include "game/entities/enemy.h"
#include "core/render/renderer.h"
#include <glm/gtc/matrix_transform.hpp>

float calculate_matrix_difference(const glm::mat4& model, const glm::mat4& prevModel) {
    // 1. 차이 행렬 D 계산
    glm::mat4 diff = model - prevModel;

    // 2. 프로베니우스 노름을 계산합니다 (모든 원소의 제곱 합의 제곱근).
    float sum_sq = 0.0f;

    // GLM mat4는 4개의 vec4(컬럼 벡터)로 구성됩니다.
    for (int i = 0; i < 4; ++i) {
        // glm::dot(vec, vec)는 벡터의 길이 제곱(L2 Norm Squared)을 반환합니다.
        // 이는 D 행렬의 i번째 컬럼에 있는 원소들의 제곱 합과 같습니다.
        sum_sq += glm::dot(diff[i], diff[i]);
    }

    // 3. 최종 스칼라 차이 (제곱근 취하기)
    return std::sqrt(sum_sq); 
}

void Player::draw_shape() const {
    const auto mesh = get_mesh();
    if (!mesh)
        return;

    glm::mat4 model = get_finalMatrix();
    glm::mat4 prevModel = get_prevModelMatrix();

    if (direction == UP)
        model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(1, 0, 0));
    else if (direction == DOWN)
        model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1, 0, 0));
    else if (direction == RIGHT)
        model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0, 1, 0));
    else if (direction == LEFT)
        model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(0, 1, 0));

    model = glm::scale(model, glm::vec3(0.3f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));

    prevModel = glm::scale(prevModel, glm::vec3(0.3f));
    prevModel = glm::rotate(prevModel, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    prevModel = glm::rotate(prevModel, glm::radians(180.0f), glm::vec3(0, 0, 1));

    glm::vec4 color(1.0f, 1.0f, 1.0f, isRecovery ? 0.2f : 1.0f);
    if (diffuseTex == 0) {
        diffuseTex = gRenderer.get_or_load_texture("assets/textures/diffuse_jet.png");
    }
    if (normalTex == 0) {
        normalTex = gRenderer.get_or_load_texture("assets/textures/normal_quilt.png");
        hasNormalMap = (normalTex != 0);
    }

    gRenderer.draw_mesh(*mesh, model, prevModel, color, true, diffuseTex, normalTex, hasNormalMap);
}

void Player::update_logic(float deltaTime) {
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
    velocity = 20;    
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
