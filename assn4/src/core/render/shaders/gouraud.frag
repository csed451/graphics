#version 330 core

in vec3 vLighting;
in vec2 vTexcoord;

uniform vec4 uColor;
uniform int uUseTexture;
uniform sampler2D uDiffuseMap;

out vec4 FragColor;

void main() {
    vec3 baseColor = (uUseTexture == 1) ? texture(uDiffuseMap, vTexcoord).rgb : uColor.rgb;
    vec3 finalColor = vLighting * baseColor;
    FragColor = vec4(finalColor, uColor.a);
}
