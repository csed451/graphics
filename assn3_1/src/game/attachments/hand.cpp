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

    glColor3f(0.9f, 0.9f, 0.9f);
    const GLfloat scaleFactor = 10.0f;

    glPushMatrix();
    glScalef(scaleFactor, scaleFactor, scaleFactor);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    if (isLeftHand)
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    mesh->draw();
    glPopMatrix();
}

void Hand::update(float deltaTime) {
    if (!get_isActive())
        return;

    animationTime += deltaTime;

    float targetSwing = std::sin(animationTime * swingFrequency + phaseOffset) * swingAmplitude;
    float deltaSwing = targetSwing - currentSwing;
    rotate_local(deltaSwing, FORWARD);
    currentSwing = targetSwing;

    // float pulse = 1.0f + pulseAmplitude * (0.5f * (std::sin(animationTime * swingFrequency * 1.5f + phaseOffset) + 1.0f));
    // float scaleFactor = pulse / currentScale;
    // scale_local(glm::vec3(scaleFactor, scaleFactor, 1.0f));
    // currentScale = pulse;
}

void Hand::deactivate() {
    set_isActive(false);
    set_isVisible(false);
}

void Hand::reset() {
    init(initialPos, initialAngle, initialAxis, initialSize, initialCenter);
    set_isActive(true);
    set_isVisible(true);
    animationTime = 0.0f;
    currentSwing = 0.0f;
    currentScale = 1.0f;
}
