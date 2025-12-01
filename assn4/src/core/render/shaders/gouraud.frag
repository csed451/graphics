#version 330 core

in vec3 vLighting;
in vec2 vTexcoord;

uniform vec4 uColor;
uniform int uUseTexture;
uniform sampler2D uDiffuseMap;

out vec4 FragColor;

void main() {
    vec3 texSample = (uUseTexture == 1) ? texture(uDiffuseMap, vTexcoord).rgb : vec3(1.0);
    vec3 baseColor = texSample * uColor.rgb;
    vec3 finalColor = vLighting * baseColor;
    FragColor = vec4(finalColor, uColor.a);
}
