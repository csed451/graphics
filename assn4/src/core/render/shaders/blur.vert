#version 330 core
layout (location = 0) in vec2 aPos;       // Quad의 정점 위치 (보통 -1.0 ~ 1.0)
layout (location = 1) in vec2 aTexCoords; // 텍스처 좌표 (0.0 ~ 1.0)

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    TexCoords = aTexCoords;
}