#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat3 uNormalMatrix;
uniform mat4 uLightSpaceMatrix;

out vec3 vNormal;
out vec3 vWorldPos;
out vec2 vTexcoord;
out vec4 vFragPosLightSpace;

void main() {
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    gl_Position = uProj * uView * worldPos;
    vWorldPos = worldPos.xyz;
    vNormal = normalize(uNormalMatrix * aNormal);
    vTexcoord = aTexcoord;

    vFragPosLightSpace = uLightSpaceMatrix * worldPos;
}
