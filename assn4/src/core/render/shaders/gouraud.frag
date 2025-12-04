#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragVelocity;

// for motion blur
in vec4 vClipPos; 
in vec4 vPrevClipPos;

in vec3 vLighting;
in vec2 vTexcoord;

uniform vec4 uColor;
uniform int uUseTexture;
uniform sampler2D uDiffuseMap;

void main() {
    vec3 texSample = (uUseTexture == 1) ? texture(uDiffuseMap, vTexcoord).rgb : vec3(1.0);
    vec3 baseColor = texSample * uColor.rgb;
    vec3 finalColor = vLighting * baseColor;
    FragColor = vec4(finalColor, uColor.a);

    vec2 ndcPos = vClipPos.xy / vClipPos.w;
    vec2 ndcPrevPos = vPrevClipPos.xy / vPrevClipPos.w;
    vec2 screenPos = ndcPos * 0.5 + 0.5;
    vec2 screenPrevPos = ndcPrevPos * 0.5 + 0.5;
    FragVelocity = vec4(screenPos - screenPrevPos, 0.0, 1);

}
