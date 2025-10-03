#include <cmath>

#include "lower.h"

Lower::Lower(
    glm::vec3 _pos,
    GLfloat _angle,
    glm::vec3 _axis,
    glm::vec3 _size,
    glm::vec3 _center,
    Object* _parent,
    float _swingAmplitude,
    float _swingFrequency,
    float _phaseOffset,
    float _handPhaseOffset
) : Object(_pos, _angle, _axis, _size, _center),
    hand(glm::vec3(0, 2.5f, 0), 0, FORWARD, glm::vec3(2), ZERO, this,
         _swingAmplitude * 1.15f, _swingFrequency * 1.4f, _handPhaseOffset, 0.25f),
    swingAmplitude(_swingAmplitude),
    swingFrequency(_swingFrequency),
    phaseOffset(_phaseOffset),
    initialPos(_pos),
    initialAngle(_angle),
    initialAxis(_axis),
    initialSize(_size),
    initialCenter(_center) {
    if (_parent) set_parent(_parent);
}

void Lower::draw_shape() const {
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
        glVertex3f(-0.5f, -0.4f, 0.0f);
        glVertex3f(0.5f, -0.4f, 0.0f);
        glVertex3f(0.7f, 2.0f, 0.0f);
        glVertex3f(-0.7f, 2.0f, 0.0f);
    glEnd();
}

void Lower::draw() const {
    Object::draw();
    hand.draw();
}

void Lower::update(float deltaTime) {
    if (!get_isActive())
        return;

    animationTime += deltaTime;

    float targetSwing = std::sin(animationTime * swingFrequency + phaseOffset) * swingAmplitude;
    float deltaSwing = targetSwing - currentSwing;
    rotate_local(deltaSwing, FORWARD);
    currentSwing = targetSwing;

    hand.update(deltaTime);
}

void Lower::deactivate() {
    set_isActive(false);
    set_isVisible(false);
    hand.deactivate();
}

void Lower::reset() {
    init(initialPos, initialAngle, initialAxis, initialSize, initialCenter);
    set_isActive(true);
    set_isVisible(true);
    animationTime = 0.0f;
    currentSwing = 0.0f;
    hand.reset();
}
