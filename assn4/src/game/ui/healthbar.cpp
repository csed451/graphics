#include <algorithm>

#include "healthbar.h"
#include "game/entities/enemy.h"
#include "core/render/renderer.h"
#include <glm/gtc/matrix_transform.hpp>

Healthbar::Healthbar(
    glm::vec3 _pos, GLfloat _angle, glm::vec3 _axis, glm::vec3 _size, glm::vec3 _center, Object* _parent
) : Object(_pos, _angle, _axis, _size, _center),
    initialPos(_pos),
    initialAngle(_angle),
    initialAxis(_axis),
    initialSize(_size),
    initialCenter(_center),
    parent(static_cast<Enemy*>(_parent)) { 
    set_mesh(load_mesh("assets/models/healthbar_box.obj"));
    if (_parent) set_parent(_parent);
}

void Healthbar::draw_shape() const {
    const auto mesh = get_mesh();

    const float offset = 0.2f;
    const float start_x = -5.0f;
    const float start_y = 0.0f;
    const float bar_width = 10.0f;
    const float bar_height = 0.8f;
    const float bg_depth = 0.35f;
    const float fill_depth = 0.25f;

    float health_ratio = 1.0f;
    if (parent)
        health_ratio = static_cast<float>(parent->get_heart()) / ENEMY_MAX_HEART;
    health_ratio = std::clamp(health_ratio, 0.0f, 1.0f);

    const float center_x = start_x + bar_width * 0.5f;
    const float center_y = start_y + bar_height * 0.5f;

    const float bg_width = bar_width + offset * 2.0f;
    const float bg_height = bar_height + offset * 2.0f;
    glm::mat4 base = get_finalMatrix();
    glm::mat4 bgModel = glm::translate(base, glm::vec3(center_x, center_y, -bg_depth * 0.5f));
    bgModel = glm::scale(bgModel, glm::vec3(bg_width, bg_height, bg_depth));
    gRenderer.draw_mesh(*mesh, bgModel, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f), false);

    if (health_ratio <= 0.0f)
        return;

    const float fill_width = bar_width * health_ratio;
    glm::mat4 fillModel = glm::translate(base, glm::vec3(start_x + fill_width * 0.5f, center_y, fill_depth * 0.5f));
    fillModel = glm::scale(fillModel, glm::vec3(fill_width, bar_height, fill_depth));
    gRenderer.draw_mesh(*mesh, fillModel, glm::vec4(1.0f, health_ratio, 0.0f, 1.0f), false);
}

void Healthbar::deactivate() {
    set_isActive(false);
    set_isVisible(false);
}

void Healthbar::reset() {
    init(initialPos, initialAngle, initialAxis, initialSize, initialCenter);
    set_isActive(true);
    set_isVisible(true);
}
