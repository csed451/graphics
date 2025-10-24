#pragma once

#include "core/base/object.h"
#include "escort_plane.h"

class Lower : public Object {
private:
    EscortPlane escortPlane;
    float animationTime = 0.0f;
    float currentSwing = 0.0f;
    float swingAmplitude;
    float swingFrequency;
    float phaseOffset;
    float pendingParentRotation = 0.0f;

    glm::vec3 initialPos;
    GLfloat   initialAngle;
    glm::vec3 initialAxis;
    glm::vec3 initialSize;
    glm::vec3 initialCenter;

public:
    Lower(
            glm::vec3 _pos = ZERO, 
            GLfloat _angle = 0, 
            glm::vec3 _axis = UP, 
            glm::vec3 _size = glm::vec3(1), 
            glm::vec3 _center = ZERO,
            Object* _parent = nullptr,
            float _swingAmplitude = 25.0f,
            float _swingFrequency = 1.8f,
            float _phaseOffset = 0.0f,
            float _planePhaseOffset = 0.0f,
            bool _isLeftPlane = false
        );

    void draw_shape() const override;
    void update(float deltaTime);
    void add_parent_rotation_delta(float deltaDegrees);
    void detach_plane();
    bool is_plane_detached() const { return escortPlane.is_detached(); }
    
    void deactivate();
    void reset();
};
