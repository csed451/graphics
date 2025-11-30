#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoord;

// Lighting constants (matches lecture notation)
const float kA = 0.2;          // ambient coefficient
const float kD = 1.0;          // diffuse coefficient
const float kS = 0.2;          // specular coefficient
const float shininess = 32.0;  // specular exponent
const float attC1 = 1.0;       // attenuation constant term
const float attC2 = 0.08;      // attenuation linear term
const float attC3 = 0.032;     // attenuation quadratic term

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat3 uNormalMatrix;
uniform vec4 uColor;
uniform int uUseLighting;
uniform vec3 uViewPos;

struct DirLight { vec3 direction; vec3 color; float intensity; };
struct PointLight { vec3 position; vec3 color; float intensity; };
uniform DirLight uDirLight;
uniform PointLight uPointLight;

out vec3 vLighting;
out vec2 vTexcoord;

vec3 apply_light(vec3 N, vec3 worldPos) {
    vec3 viewDir = normalize(uViewPos - worldPos);
    vec3 colorAccum = vec3(kA); // ambient factor

    vec3 Ld = normalize(-uDirLight.direction);
    float diffD = max(dot(N, Ld), 0.0);
    vec3 halfwayD = normalize(Ld + viewDir);
    float specD = pow(max(dot(N, halfwayD), 0.0), shininess);
    colorAccum += (uDirLight.color * uDirLight.intensity) * (kD * diffD + kS * specD);

    vec3 Lp = normalize(uPointLight.position - worldPos);
    float diffP = max(dot(N, Lp), 0.0);
    vec3 halfwayP = normalize(Lp + viewDir);
    float specP = pow(max(dot(N, halfwayP), 0.0), shininess);
    float dist = length(uPointLight.position - worldPos);
    float atten = 1.0 / (attC1 + attC2 * dist + attC3 * dist * dist);
    colorAccum += (uPointLight.color * uPointLight.intensity) * (kD * diffP + kS * specP) * atten;

    return colorAccum;
}

void main() {
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    gl_Position = uProj * uView * worldPos;

    vec3 N = normalize(uNormalMatrix * aNormal);
    vLighting = (uUseLighting == 0) ? vec3(1.0) : apply_light(N, worldPos.xyz);
    vTexcoord = aTexcoord;
}
