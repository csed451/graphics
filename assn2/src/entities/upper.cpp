#include <cmath>

#include "upper.h"

Upper::Upper(
    glm::vec3 _pos, GLfloat _angle, glm::vec3 _axis, glm::vec3 _size, glm::vec3 _center, Object* _parent,
    float _swingAmplitude, float _swingFrequency, float _phaseOffset,
    float _lowerSwingAmplitude, float _lowerSwingFrequency, float _lowerPhaseOffset
) : Object(_pos, _angle, _axis, _size, _center),
    lowerArm(glm::vec3(0, 3.5f, 0), 0, FORWARD, _size, ZERO, this, 
            _lowerSwingAmplitude, _lowerSwingFrequency, _lowerPhaseOffset, _lowerPhaseOffset * 1.1f),
    swingAmplitude(_swingAmplitude), swingFrequency(_swingFrequency), phaseOffset(_phaseOffset),
    initialPos(_pos), initialAngle(_angle), initialAxis(_axis), initialSize(_size), initialCenter(_center) {
    if (_parent) set_parent(_parent);
}

void Upper::draw_shape() const {
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
        glVertex3f(-0.3f, 0.0f, 0.0f);
        glVertex3f(0.3f, 0.0f, 0.0f);
        glVertex3f(0.5f, 2.5f, 0.0f);
        glVertex3f(-0.5f, 2.5f, 0.0f);
    glEnd();
}

void Upper::update(float deltaTime) {
    if (!get_isActive())
        return; 

    animationTime += deltaTime;

    float targetSwing = std::sin(animationTime * swingFrequency + phaseOffset) * swingAmplitude;
    float deltaSwing = targetSwing - currentSwing;
    rotate_local(deltaSwing, FORWARD);
    currentSwing = targetSwing;

    lowerArm.update(deltaTime);
}

void Upper::draw() const {
    Object::draw();
    lowerArm.draw();
}

void Upper::reset_pose() {
    init(initialPos, initialAngle, initialAxis, initialSize, initialCenter);
    animationTime = 0.0f;
    currentSwing = 0.0f;
    lowerArm.reset_pose();
}
