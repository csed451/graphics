#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture; 
uniform sampler2D velocityTexture; 
uniform int useVelocity;

// 1. 샘플링 횟수를 극단적으로 증가시켜 블러 품질 향상
const int SAMPLES = 4; 


void main()
{
    vec2 velocity = texture(velocityTexture, TexCoords).rg;

    if (useVelocity == 0 || length(velocity) < 0.000001)  
    {
        FragColor = texture(screenTexture, TexCoords);
        return;
    }

    vec3 finalColor = vec3(0.0);
    vec2 currentCoords = TexCoords;
    vec2 sampleStep = -velocity / float(SAMPLES);
    currentCoords += velocity*0.5;

    for (int i = 0; i < SAMPLES; i++)
    {
        vec2 clampedCoords = clamp(currentCoords, 0.0, 1.0);
        finalColor += texture(screenTexture, clampedCoords).rgb;
        currentCoords += sampleStep;
    }

    finalColor /= float(SAMPLES);
    
    FragColor = vec4(finalColor, 1.0);
}