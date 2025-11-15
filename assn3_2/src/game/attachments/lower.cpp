#include <cmath>

#include "lower.h"
#include "core/render/mesh.h"
#include "core/render/renderer.h"
#include <glm/gtc/matrix_transform.hpp>

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
    float _planePhaseOffset,
    bool _isLeftPlane
) : Object(_pos, _angle, _axis, _size, _center),
    escortPlane(glm::vec3(0, 3.5f, 0), 0, FORWARD, glm::vec3(2), ZERO, this,
                _swingAmplitude * 1.15f, _swingFrequency * 1.4f, _planePhaseOffset, 0.25f, _isLeftPlane),
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

    glm::mat4 model = get_finalMatrix();
    gRenderer.draw_mesh(*mesh, model, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
}

void Lower::update_logic(float deltaTime) {
    animationTime += deltaTime;
    float inheritedDelta = pendingParentRotation;
    pendingParentRotation = 0.0f;

    float targetSwing = std::sin(animationTime * swingFrequency + phaseOffset) * swingAmplitude;
    float deltaSwing = targetSwing - currentSwing;
    rotate_local(deltaSwing, FORWARD);
    currentSwing = targetSwing;

    if (!escortPlane.is_detached()) {
        float netDelta = inheritedDelta + deltaSwing;
        escortPlane.apply_parent_rotation_correction(netDelta);
    }

}

void Lower::deactivate() {
    set_isActive(false);
    set_isVisible(false);
    escortPlane.deactivate();
}

void Lower::reset() {
    init(initialPos, initialAngle, initialAxis, initialSize, initialCenter);
    set_isActive(true);
    set_isVisible(true);
    animationTime = 0.0f;
    currentSwing = 0.0f;
    pendingParentRotation = 0.0f;
    escortPlane.reset();
}

void Lower::add_parent_rotation_delta(float deltaDegrees) {
    if (escortPlane.is_detached())
        return;
    pendingParentRotation += deltaDegrees;
}

void Lower::detach_plane() {
    escortPlane.detach_to_world();
}
