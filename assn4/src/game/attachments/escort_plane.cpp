#include <cmath>

#include "escort_plane.h"
#include "core/base/scene_node.h"
#include "core/globals/game_constants.h"
#include "core/render/mesh.h"
#include "core/render/renderer.h"

#include <glm/gtc/matrix_transform.hpp>

EscortPlane::EscortPlane(
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
    bool _isLeftPlane
) : Object(_pos, _angle, _axis, _size, _center),
    swingAmplitude(_swingAmplitude),
    swingFrequency(_swingFrequency),
    phaseOffset(_phaseOffset),
    pulseAmplitude(_pulseAmplitude),
    isLeftPlane(_isLeftPlane),
    initialPos(_pos),
    initialAngle(_angle),
    initialAxis(_axis),
    initialSize(_size),
    initialCenter(_center) {
    if (_parent) {
        initialParent = _parent;
        set_parent(_parent);
    }
    set_mesh(load_mesh("assets/models/starship.obj"));
}

void EscortPlane::draw_shape() const {
    const auto mesh = get_mesh();
    if (!mesh)
        return;

    glm::mat4 model = get_finalMatrix();
    model = glm::scale(model, glm::vec3(10.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 0, 1));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
    if (isLeftPlane)
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));

    if (diffuseTex == 0)
        diffuseTex = gRenderer.get_or_load_texture("assets/textures/diffuse_starship.png");
    if (normalTex == 0) {
        normalTex = gRenderer.get_or_load_texture("assets/textures/normal_quilt.png");
        hasNormalMap = (normalTex != 0);
    }

    gRenderer.draw_mesh(*mesh, model, get_prevModelMatrix(), glm::vec4(1.0f), true, diffuseTex, normalTex, hasNormalMap);
}

void EscortPlane::update_logic(float deltaTime) {
    animationTime += deltaTime;

    if (isDetached) {
        detachedWorldMatrix = glm::translate(detachedWorldMatrix, travelDirection * travelSpeed * deltaTime);
        set_modelMatrix(detachedWorldMatrix);

        if (is_outside_window(get_pos())) {
            set_isActive(false);
            set_isVisible(false);
        }
        return;
    }

    // float pulse = 1.0f + pulseAmplitude * (0.5f * (std::sin(animationTime * swingFrequency * 1.5f + phaseOffset) + 1.0f));
    // float scaleFactor = pulse / currentScale;
    // scale_local(glm::vec3(scaleFactor, scaleFactor, 1.0f));
    // currentScale = pulse;
}

void EscortPlane::apply_parent_rotation_correction(float deltaDegrees) {
    if (isDetached)
        return;

    if (std::abs(deltaDegrees) < 1e-4f)
        return;

    rotate_local(-deltaDegrees, FORWARD);
}

void EscortPlane::detach_to_world() {
    if (isDetached)
        return;

    detachedWorldMatrix = get_finalMatrix();
    set_parent(nullptr);
    set_modelMatrix(detachedWorldMatrix);
    set_parent(&sceneRoot, true);

    isDetached = true;
    travelDirection = isLeftPlane ? LEFT : RIGHT;
    currentScale = 1.0f;
}

void EscortPlane::deactivate() {
    // If already detached and falling, let update_logic handle lifetime
    if (isDetached)
        return;
    set_isActive(false);
    set_isVisible(false);
}

void EscortPlane::reset() {
    if (initialParent)
        set_parent(initialParent);

    init(initialPos, initialAngle, initialAxis, initialSize, initialCenter);
    set_isActive(true);
    set_isVisible(true);

    animationTime = 0.0f;
    currentScale = 1.0f;
    isDetached = false;
    travelDirection = glm::vec3(0.0f);
    detachedWorldMatrix = glm::mat4(1.0f);
}
