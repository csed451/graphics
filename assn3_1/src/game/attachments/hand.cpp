#include <cmath>

#include "hand.h"

Hand::Hand(
    glm::vec3 _pos,
    GLfloat _angle,
    glm::vec3 _axis,
    glm::vec3 _size,
    glm::vec3 _center,
    Object* _parent,
    float _swingAmplitude,
    float _swingFrequency,
    float _phaseOffset,
    float _pulseAmplitude,
    bool _isLeftHand
) : Object(_pos, _angle, _axis, _size, _center),
    swingAmplitude(_swingAmplitude),
    swingFrequency(_swingFrequency),
    phaseOffset(_phaseOffset),
    pulseAmplitude(_pulseAmplitude),
    isLeftHand(_isLeftHand),
    initialPos(_pos),
    initialAngle(_angle),
    initialAxis(_axis),
    initialSize(_size),
    initialCenter(_center) {
    if (_parent) set_parent(_parent);
    set_mesh(load_mesh("assets/paperplane.obj"));
}

void Hand::draw_shape() const {
    const auto mesh = get_mesh();
    if (!mesh)
        return;

    glColor4f(0.85f, 0.15f, 0.15f, 0.9f);
    const GLfloat scaleFactor = 10.0f;

    glPushMatrix();
    glScalef(scaleFactor, scaleFactor, scaleFactor);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    if (isLeftHand)
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    mesh->draw();
    glPopMatrix();
}

void Hand::update(float /*deltaTime*/) {
    if (!get_isActive())
        return;
}

void Hand::apply_parent_rotation_correction(float deltaDegrees) {
    if (std::abs(deltaDegrees) < 1e-4f)
        return;
    rotate_local(-deltaDegrees, FORWARD);
}

void Hand::deactivate() {
    set_isActive(false);
    set_isVisible(false);
}

void Hand::reset() {
    init(initialPos, initialAngle, initialAxis, initialSize, initialCenter);
    set_isActive(true);
    set_isVisible(true);
    currentScale = 1.0f;
}
