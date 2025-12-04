#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec2 FragVelocity;

in vec3 vLighting;
in vec2 vTexcoord;
in vec4 vClipPos; 
in vec4 vPrevClipPos;
uniform bool uUseVelocity;

uniform vec4 uColor;
uniform int uUseTexture;
uniform sampler2D uDiffuseMap;

void main() {
    vec3 texSample = (uUseTexture == 1) ? texture(uDiffuseMap, vTexcoord).rgb : vec3(1.0);
    vec3 baseColor = texSample * uColor.rgb;
    vec3 finalColor = vLighting * baseColor;
    FragColor = vec4(finalColor, uColor.a);

    if (uUseVelocity) {
        // (1) Perspective Divide: Clip Space -> NDC (-1 ~ 1)
        vec2 ndcPos = vClipPos.xy / vClipPos.w;
        vec2 ndcPrevPos = vPrevClipPos.xy / vPrevClipPos.w;

        // (2) Remap to Texture Space: NDC -> UV (0 ~ 1)
        vec2 screenPos = ndcPos * 0.5 + 0.5;
        vec2 screenPrevPos = ndcPrevPos * 0.5 + 0.5;

        // (3) Velocity Vector = Current - Previous
        FragVelocity = screenPos - screenPrevPos;
    } 
    else {
        // 블러 대상이 아니면 속도는 0
        FragVelocity = vec2(0.0, 0.0);
    }
}
