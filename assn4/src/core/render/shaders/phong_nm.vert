#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoord;
layout (location = 3) in vec3 aTangent;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat3 uNormalMatrix;

out vec3 vWorldPos;
out vec3 vNormal;
out vec3 vTangent;
out vec2 vTexcoord;

void main() {
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    gl_Position = uProj * uView * worldPos;
    vWorldPos = worldPos.xyz;
    vNormal = normalize(uNormalMatrix * aNormal);
    vTangent = normalize(uNormalMatrix * aTangent);
    vTexcoord = aTexcoord;
}
