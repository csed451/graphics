#version 330 core

in vec3 vNormal;
in vec3 vWorldPos;

uniform vec4 uColor;
uniform int uUseLighting;
uniform vec3 uViewPos;

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
uniform PointLight uPointLight;

out vec4 FragColor;

vec3 apply_light(vec3 N, vec3 viewDir) {
    vec3 colorAccum = vec3(0.2) * uColor.rgb; // ambient

    vec3 Ld = normalize(-uDirLight.direction);
    float diffD = max(dot(N, Ld), 0.0);
    vec3 halfwayD = normalize(Ld + viewDir);
    float specD = pow(max(dot(N, halfwayD), 0.0), 32.0);
    colorAccum += (uDirLight.color * uDirLight.intensity) * (diffD * uColor.rgb + 0.2 * specD);

    vec3 Lp = normalize(uPointLight.position - vWorldPos);
    float diffP = max(dot(N, Lp), 0.0);
    vec3 halfwayP = normalize(Lp + viewDir);
    float specP = pow(max(dot(N, halfwayP), 0.0), 32.0);
    float dist = length(uPointLight.position - vWorldPos);
    float atten = 1.0 / (1.0 + 0.08 * dist + 0.032 * dist * dist); // attenuation to localize point light
    colorAccum += (uPointLight.color * uPointLight.intensity) * (diffP * uColor.rgb + 0.2 * specP) * atten;

    return colorAccum;
}

void main() {
    if (uUseLighting == 0) {
        FragColor = uColor;
        return;
    }

    vec3 N = normalize(vNormal);
    vec3 viewDir = normalize(uViewPos - vWorldPos);
    vec3 lit = apply_light(N, viewDir);
    FragColor = vec4(lit, uColor.a);
}
