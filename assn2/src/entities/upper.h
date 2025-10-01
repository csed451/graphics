#pragma once

#include "object.h"
#include "objectPool.h"
#include "lower.h"


class Upper : public Object {
private:
    Lower lowerArm;

    float swingAmplitude;
    float swingFrequency;
    float phaseOffset;
    
    float animationTime = 0.0f;
    float currentSwing = 0.0f;

    glm::vec3 initialPos;
    GLfloat initialAngle;
    glm::vec3 initialAxis;
    glm::vec3 initialSize; 
    glm::vec3 initialCenter;

public:
    Upper(
            glm::vec3 _pos = ZERO, 
            GLfloat _angle = 0, 
            glm::vec3 _axis = UP, 
            glm::vec3 _size = glm::vec3(1), 
            glm::vec3 _center = glm::vec3(0, -0.5, 0),
            Object* _parent = nullptr,
            float _swingAmplitude = 15.0f,
            float _swingFrequency = 1.0f,
            float _phaseOffset = 0.0f,
            float _lowerSwingAmplitude = 25.0f,
            float _lowerSwingFrequency = 1.8f,
            float _lowerPhaseOffset = 0.0f
        );

    void draw_shape() const override;
    void draw() const;
    void update(float deltaTime);
    void reset_pose();
};
