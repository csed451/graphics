#pragma once

#include "core/base/object.h"

class Hand : public Object {
private:
    float currentScale = 1.0f;
    float swingAmplitude;
    float swingFrequency;
    float phaseOffset;
    float pulseAmplitude;
    bool isLeftHand;

    glm::vec3 initialPos;
    GLfloat initialAngle;
    glm::vec3 initialAxis;
    glm::vec3 initialSize;
    glm::vec3 initialCenter;

public:
    Hand(
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
        bool _isLeftHand = false
    );

    void draw_shape() const override;
    void update(float deltaTime);
    void apply_parent_rotation_correction(float deltaDegrees);
    void deactivate();
    void reset();
};
