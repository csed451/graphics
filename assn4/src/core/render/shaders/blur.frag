#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D colorTexture; 
uniform sampler2D velocityTexture; 
uniform int useVelocity;

const int SAMPLES = 4; 


void main()
{
    vec2 velocity = texture(velocityTexture, TexCoords).rg;

    if (useVelocity == 0 || length(velocity) < 0.000001) {
        FragColor = texture(colorTexture, TexCoords);
        return;
    }

    vec2 sampleStep = -velocity / float(SAMPLES);

    vec2 currentCoords = TexCoords;
    vec3 finalColor = vec3(0.0);

    for (int i = 0; i < SAMPLES; i++) {
        currentCoords += sampleStep;
        finalColor += texture(colorTexture, clamp(currentCoords, 0.0, 1.0)).rgb;
    }
    
    FragColor = vec4(finalColor/float(SAMPLES), 1.0);
}