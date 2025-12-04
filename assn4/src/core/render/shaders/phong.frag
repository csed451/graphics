#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragVelocity;

// for motion blur
in vec4 vClipPos; 
in vec4 vPrevClipPos;

in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vTexcoord;
in vec4 vFragPosLightSpace;

uniform vec4 uColor;
uniform int uUseLighting;
uniform vec3 uViewPos;
uniform int uUseTexture;
uniform sampler2D uDiffuseMap;

// shadowmap texture
uniform sampler2D uShadowMap;
uniform int uUseShadow;
const float bias = 0.005;

// Lighting constants (matches lecture notation)
const float kA = 0.2;          // ambient coefficient
const float kD = 1.0;          // diffuse coefficient
const float kS = 0.2;          // specular coefficient
const float shininess = 32.0;  // specular exponent
const float attC1 = 1.0;       // attenuation constant term
const float attC2 = 0.08;      // attenuation linear term
const float attC3 = 0.032;     // attenuation quadratic term
const int   MAX_POINT_LIGHTS = 4;

struct DirLight {
    vec3 direction;
    vec3 color;
    float intensity;
};
struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform DirLight uDirLight;
uniform PointLight uPointLights[MAX_POINT_LIGHTS];
uniform int uPointLightCount;


float calculate_shadow()
{
    if (vFragPosLightSpace.w <= 0.0) return 1.0; 

    vec3 projCoords = vFragPosLightSpace.xyz / vFragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.x > 1.0 || projCoords.x < 0.0 || 
        projCoords.y > 1.0 || projCoords.y < 0.0 ||
        projCoords.z > 1.0 || projCoords.z < 0.0) 
        return 1.0;
    
    if (projCoords.z > texture(uShadowMap, projCoords.xy).r + bias)
        return 0.0;
    
    return 1.0;
}

vec3 apply_light(vec3 baseColor, vec3 N, vec3 viewDir) {
    float shadowFactor = uUseShadow == 1 ? calculate_shadow() : 1.0;

    vec3 colorAccum = kA * baseColor; // ambient

    vec3 Ld = normalize(-uDirLight.direction);
    float diffD = max(dot(N, Ld), 0.0);
    vec3 halfwayD = normalize(Ld + viewDir);
    float specD = pow(max(dot(N, halfwayD), 0.0), shininess);
    colorAccum += (uDirLight.color * uDirLight.intensity) * (kD * diffD * baseColor + kS * specD) * shadowFactor;

    for (int i = 0; i < uPointLightCount; ++i) {
        PointLight pl = uPointLights[i];
        vec3 Lp = normalize(pl.position - vWorldPos);
        float diffP = max(dot(N, Lp), 0.0);
        vec3 halfwayP = normalize(Lp + viewDir);
        float specP = pow(max(dot(N, halfwayP), 0.0), shininess);
        float dist = length(pl.position - vWorldPos);
        float atten = 1.0 / (attC1 + attC2 * dist + attC3 * dist * dist);
        colorAccum += (pl.color * pl.intensity) * (kD * diffP * baseColor + kS * specP) * atten;
    }

    return colorAccum;
}

void main() {
    vec3 texSample = (uUseTexture == 1) ? texture(uDiffuseMap, vTexcoord).rgb : vec3(1.0);
    vec3 baseColor = texSample * uColor.rgb;
    if (uUseLighting == 0) {
        FragColor = vec4(baseColor, uColor.a);
        return;
    }

    vec3 N = normalize(vNormal);
    vec3 viewDir = normalize(uViewPos - vWorldPos);
    vec3 lit = apply_light(baseColor, N, viewDir);
    FragColor = vec4(lit, uColor.a);

    vec2 ndcPos = vClipPos.xy / vClipPos.w;
    vec2 ndcPrevPos = vPrevClipPos.xy / vPrevClipPos.w;
    vec2 screenPos = ndcPos * 0.5 + 0.5;
    vec2 screenPrevPos = ndcPrevPos * 0.5 + 0.5;
    FragVelocity = vec4(screenPos - screenPrevPos, 0.0, 1);
}
