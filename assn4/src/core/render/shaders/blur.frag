#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture; 
uniform sampler2D velocityTexture; 

// 1. 샘플링 횟수를 극단적으로 증가시켜 블러 품질 향상
const int SAMPLES = 64; 

// 2. 블러의 길이를 극단적으로 늘리기 위한 상수 (5배 증폭)
const float EXTREME_STRENGTH = 5.0; 

void main()
{
    // 1. 현재 픽셀의 속도 벡터 (텍스처 좌표 공간)를 가져옵니다.
    vec2 velocity = texture(velocityTexture, TexCoords).rg;
    
    // 🚨 극단적인 블러 효과를 위해 속도 벡터에 강도 적용
    velocity *= EXTREME_STRENGTH; 

    // 성능 개선을 위한 임계값 조정 (0.0001에서 0.00001로 낮춤)
    if (length(velocity) < 0.00001)  
    {
        FragColor = texture(screenTexture, TexCoords);
        return;
    }

    // 2. 모션 벡터를 따라 샘플링을 시작합니다.
    vec3 finalColor = vec3(0.0);
    vec2 currentCoords = TexCoords;
    
    // 증폭된 속도 벡터를 SAMPLES로 나눔
    vec2 sampleStep = velocity / float(SAMPLES);
    
    // 블러가 중심에 오도록 시작 지점을 조정합니다.
    currentCoords -= sampleStep * (float(SAMPLES) / 2.0);

    // 3. 샘플링 및 누적 (Accumulation)
    for (int i = 0; i < SAMPLES; i++)
    {
        // 텍스처 좌표를 [0, 1] 범위로 클램프
        vec2 clampedCoords = clamp(currentCoords, 0.0, 1.0);
        
        // 해당 지점에서 색상을 샘플링하여 누적
        finalColor += texture(screenTexture, clampedCoords).rgb;
        
        currentCoords += sampleStep;
    }

    // 4. 평균을 내어 최종 색상을 결정합니다.
    finalColor /= float(SAMPLES);
    
    // 5. 최종 출력
    FragColor = vec4(finalColor, 1.0);
    FragColor = texture(velocityTexture, TexCoords);
}