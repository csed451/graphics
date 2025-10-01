#include "healthbar.h"
#include "enemy.h"

Healthbar::Healthbar(
    glm::vec3 _pos, GLfloat _angle, glm::vec3 _axis, glm::vec3 _size, glm::vec3 _center, Object* _parent
) : initialPos(_pos), initialAngle(_angle), initialAxis(_axis), initialSize(_size), initialCenter(_center),
    Object(_pos, _angle, _axis, _size, _center), parent(static_cast<Enemy*>(_parent)) { 
    if (_parent) set_parent(_parent);
}

void Healthbar::draw_shape() const {
    float offset = 0.2f;
    float start_x = -5.0f;
    float start_y = 0.0f;
    float bar_width = 10.0f;
    float bar_height = 0.8f;
    float health_ratio = (float) parent->get_heart() / ENEMY_MAX_HEART;

    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
        glVertex2f(start_x - offset, start_y - offset);
        glVertex2f(start_x + bar_width + offset, start_y - offset);
        glVertex2f(start_x + bar_width + offset, start_y + bar_height + offset);
        glVertex2f(start_x - offset, start_y + bar_height + offset);
    glEnd();

    // Remaining health bar
    glColor3f(1.0f, health_ratio, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(start_x, start_y);
        glVertex2f(start_x + bar_width * health_ratio, start_y);
        glVertex2f(start_x + bar_width * health_ratio, start_y + bar_height);
        glVertex2f(start_x, start_y + bar_height);
    glEnd();
}

void Healthbar::update(float deltaTime) {
    if (!get_isActive())
        return;
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
