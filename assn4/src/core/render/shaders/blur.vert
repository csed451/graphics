#version 330 core

// C++에서 사각형을 그릴 때 입력되는 버텍스 좌표 (정규화된 장치 좌표: NDC)
layout (location = 0) in vec3 aPos; 
// C++에서 사각형을 그릴 때 입력되는 텍스처 좌표
layout (location = 1) in vec2 aTexCoords;

// 프래그먼트 쉐이더로 전달할 텍스처 좌표
out vec2 TexCoords;

void main()
{
    // 입력된 텍스처 좌표를 그대로 전달
    TexCoords = aTexCoords;
    // 버텍스 좌표를 최종 출력 위치로 사용 (Quad가 화면 전체를 덮게 함)
    gl_Position = vec4(aPos, 1.0); 
}