#pragma once

#include "core/base/object.h"
#include "hand.h"

class Lower : public Object {
private:
    Hand hand;
    float animationTime = 0.0f;
    float currentSwing = 0.0f;
    float swingAmplitude;
    float swingFrequency;
    float phaseOffset;

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
            float _handPhaseOffset = 0.0f,
            bool _isLeftHand = false
        );

    void draw_shape() const override;
    void update(float deltaTime);
    
    void deactivate();
    void reset();
};
