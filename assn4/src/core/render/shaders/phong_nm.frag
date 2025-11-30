#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec3 vTangent;
in vec2 vTexcoord;

uniform vec4 uColor;
uniform int uUseLighting;
uniform int uUseTexture;
uniform sampler2D uDiffuseMap;
uniform int uUseNormalMap;
uniform sampler2D uNormalMap;
uniform vec3 uViewPos;

// Lighting constants (matches lecture notation)
const float kA = 0.2;          // ambient coefficient
const float kD = 1.0;          // diffuse coefficient
const float kS = 0.2;          // specular coefficient
const float shininess = 32.0;  // specular exponent
const float attC1 = 1.0;       // attenuation constant term
const float attC2 = 0.08;      // attenuation linear term
const float attC3 = 0.032;     // attenuation quadratic term

struct DirLight { vec3 direction; vec3 color; float intensity; };
struct PointLight { vec3 position; vec3 color; float intensity; };
uniform DirLight uDirLight;
uniform PointLight uPointLight;

out vec4 FragColor;

vec3 apply_light(vec3 baseColor, vec3 N, vec3 viewDir) {
    vec3 colorAccum = kA * baseColor; // ambient

    vec3 Ld = normalize(-uDirLight.direction);
    float diffD = max(dot(N, Ld), 0.0);
    vec3 halfwayD = normalize(Ld + viewDir);
    float specD = pow(max(dot(N, halfwayD), 0.0), shininess);
    colorAccum += (uDirLight.color * uDirLight.intensity) * (kD * diffD * baseColor + kS * specD);

    vec3 Lp = normalize(uPointLight.position - vWorldPos);
    float diffP = max(dot(N, Lp), 0.0);
    vec3 halfwayP = normalize(Lp + viewDir);
    float specP = pow(max(dot(N, halfwayP), 0.0), shininess);
    float dist = length(uPointLight.position - vWorldPos);
    float atten = 1.0 / (attC1 + attC2 * dist + attC3 * dist * dist);
    colorAccum += (uPointLight.color * uPointLight.intensity) * (kD * diffP * baseColor + kS * specP) * atten;

    return colorAccum;
}

vec3 sample_normal() {
    if (uUseNormalMap == 0)
        return normalize(vNormal);

    vec3 T = normalize(vTangent);
    vec3 N = normalize(vNormal);
    vec3 B = normalize(cross(N, T));

    mat3 TBN = mat3(T, B, N);
    vec3 mapN = texture(uNormalMap, vTexcoord).xyz * 2.0 - 1.0;
    return normalize(TBN * mapN);
}

void main() {
    vec3 baseColor = (uUseTexture == 1) ? texture(uDiffuseMap, vTexcoord).rgb : uColor.rgb;

    if (uUseLighting == 0) {
        FragColor = vec4(baseColor, uColor.a);
        return;
    }

    vec3 N = sample_normal();
    vec3 viewDir = normalize(uViewPos - vWorldPos);
    vec3 lit = apply_light(baseColor, N, viewDir);
    FragColor = vec4(lit, uColor.a);
}
