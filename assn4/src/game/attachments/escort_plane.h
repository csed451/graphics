#pragma once

#include <GL/glew.h>
#include "core/base/object.h"

class EscortPlane : public Object {
private:
    float animationTime = 0.0f;
    float currentScale = 1.0f;
    float swingAmplitude;
    float swingFrequency;
    float phaseOffset;
    float pulseAmplitude;
    bool isLeftPlane;
    mutable GLuint diffuseTex = 0;
    mutable GLuint normalTex = 0;
    mutable bool hasNormalMap = false;

    const float travelSpeed = 50.0f;
    bool isDetached = false;
    glm::vec3 travelDirection = glm::vec3(0.0f);
    glm::mat4 detachedWorldMatrix = glm::mat4(1.0f);
    Object* initialParent = nullptr;
    glm::vec3 initialPos;
    GLfloat initialAngle;
    glm::vec3 initialAxis;
    glm::vec3 initialSize;
    glm::vec3 initialCenter;

public:
    EscortPlane(
        glm::vec3 _pos = ZERO,
        GLfloat _angle = 0,
        glm::vec3 _axis = UP,
        glm::vec3 _size = glm::vec3(1),
        glm::vec3 _center = ZERO,
        Object* _parent = nullptr,
        float _swingAmplitude = 35.0f,
        float _swingFrequency = 2.5f,
        float _phaseOffset = 0.0f,
        float _pulseAmplitude = 0.2f,
        bool _isLeftPlane = false
    );

    void draw_shape() const override;
    void update_logic(float deltaTime) override;
    void apply_parent_rotation_correction(float deltaDegrees);
    void detach_to_world();
    bool is_detached() const { return isDetached; }

    void deactivate();
    void reset();
};
