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
    float _handPhaseOffset,
    bool _isLeftHand
) : Object(_pos, _angle, _axis, _size, _center),
    hand(glm::vec3(0, 3.5f, 0), 0, FORWARD, glm::vec3(2), ZERO, this,
         _swingAmplitude * 1.15f, _swingFrequency * 1.4f, _handPhaseOffset, 0.25f, _isLeftHand),
    swingAmplitude(_swingAmplitude),
    swingFrequency(_swingFrequency),
    phaseOffset(_phaseOffset),
    initialPos(_pos),
    initialAngle(_angle),
    initialAxis(_axis),
    initialSize(_size),
    initialCenter(_center) {
    if (_parent) set_parent(_parent);
    set_mesh(load_mesh("assets/lower_arm.obj"));
}

void Lower::draw_shape() const {
    const auto mesh = get_mesh();
    if (!mesh)
        return;

    glColor3f(0.9f, 0.9f, 0.9f);
    glPushMatrix();
    mesh->draw();
    glPopMatrix();
}

void Lower::update(float deltaTime) {
    if (!get_isActive())
        return;

    animationTime += deltaTime;
    float inheritedDelta = pendingParentRotation;
    pendingParentRotation = 0.0f;

    float targetSwing = std::sin(animationTime * swingFrequency + phaseOffset) * swingAmplitude;
    float deltaSwing = targetSwing - currentSwing;
    rotate_local(deltaSwing, FORWARD);
    currentSwing = targetSwing;

    float netDelta = inheritedDelta + deltaSwing;
    hand.apply_parent_rotation_correction(netDelta);
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
    pendingParentRotation = 0.0f;
    hand.reset();
}

void Lower::add_parent_rotation_delta(float deltaDegrees) {
    pendingParentRotation += deltaDegrees;
}
