#pragma once

#include <string>
#include <unordered_map>

#include <GL/glew.h>

class ShaderProgram {
private:
    GLuint programId = 0;                                           // init 0 (means no program)
    mutable std::unordered_map<std::string, GLint> uniformCache;    // Cache for uniform locations

    GLuint compile(GLenum type, const std::string& path);           // 단일 셰이더를 읽고 컴파일
    std::string load_source(const std::string& path);               // 텍스트 셰이더 파일 로드
    void destroy();                                                 // 기존 프로그램 캐시 해제

public:
    ShaderProgram() = default;
    ~ShaderProgram();

    // Prevent copy and move semantics
    ShaderProgram(const ShaderProgram&) = delete;                   // 복사 생성
    ShaderProgram& operator=(const ShaderProgram&) = delete;        // 복사 대입
    ShaderProgram(ShaderProgram&&) noexcept = delete;               // 이동 생성
    ShaderProgram& operator=(ShaderProgram&&) noexcept = delete;    // 이동 대입

    // Getter functions
    GLint uniform_location(const std::string& name) const; 
    GLuint id() const { return programId; }

    bool load_from_files(const std::string& vertPath, const std::string& fragPath);
    void bind() const;    
    void unbind() const;  
};
