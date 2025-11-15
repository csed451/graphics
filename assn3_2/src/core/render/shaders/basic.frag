#version 330 core

in vec3 vNormal;
in vec3 vWorldPos;

uniform vec4 uColor;
uniform int uUseLighting;

out vec4 FragColor;

void main() {
    if (uUseLighting == 0) {
        FragColor = uColor;
        return;
    }

    vec3 N = normalize(vNormal);
    vec3 L = normalize(vec3(-0.4, 0.7, 0.5));
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = uColor.rgb * diff;
    vec3 ambient = uColor.rgb * 0.2;
    FragColor = vec4(ambient + diffuse, uColor.a);
}
