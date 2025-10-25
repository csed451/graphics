#include <cmath>

#include "upper.h"
#include "core/render/mesh.h"

Upper::Upper(
    glm::vec3 _pos, 
    GLfloat _angle, 
    glm::vec3 _axis, 
    glm::vec3 _size, 
    glm::vec3 _center, 
    Object* _parent,
    float _swingAmplitude, 
    float _swingFrequency, 
    float _phaseOffset,
    float _lowerSwingAmplitude, 
    float _lowerSwingFrequency, 
    float _lowerPhaseOffset,
    bool _isLeftPlane
) : Object(_pos, _angle, _axis, _size, _center),
    lowerArm(glm::vec3(0, 3.5f, 0), 0, FORWARD, _size, ZERO, this,
             _lowerSwingAmplitude, _lowerSwingFrequency, _lowerPhaseOffset, _lowerPhaseOffset * 1.1f, _isLeftPlane),
    swingAmplitude(_swingAmplitude),
    swingFrequency(_swingFrequency),
    phaseOffset(_phaseOffset),
    initialPos(_pos),
    initialAngle(_angle),
    initialAxis(_axis),
    initialSize(_size),
    initialCenter(_center) {
    if (_parent) set_parent(_parent);
    set_mesh(load_mesh("assets/upper_arm.obj"));
}

void Upper::draw_shape() const {
    const auto mesh = get_mesh();
    if (!mesh)
        return;

    glColor3f(0.9f, 0.9f, 0.9f);
    mesh->draw();
}

void Upper::update_logic(float deltaTime) {
    animationTime += deltaTime;

    float targetSwing = std::sin(animationTime * swingFrequency + phaseOffset) * swingAmplitude;
    float deltaSwing = targetSwing - currentSwing;
    rotate_local(deltaSwing, FORWARD);
    currentSwing = targetSwing;

    lowerArm.add_parent_rotation_delta(deltaSwing);
}

void Upper::detach_plane() {
    lowerArm.detach_plane();
}

void Upper::deactivate() {
    set_isActive(false);
    set_isVisible(false);
    lowerArm.deactivate();
}

void Upper::reset() {
    init(initialPos, initialAngle, initialAxis, initialSize, initialCenter);
    set_isActive(true);
    set_isVisible(true);
    animationTime = 0.0f;
    currentSwing = 0.0f;
    lowerArm.reset();
}
