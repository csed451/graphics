#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

// C++에서 보낸 0번(Color), 1번(Velocity) 텍스처
uniform sampler2D screenTexture;
uniform sampler2D velocityTexture;

void main()
{
    // 1. 0번 텍스처(게임 화면)에서 색상만 쏙 뽑아오기
    vec3 color = texture(screenTexture, TexCoords).rgb;
    vec4 velocity = texture(velocityTexture, TexCoords);
    
    // 2. 화면에 그대로 출력
    FragColor = vec4(color, 1.0);
}