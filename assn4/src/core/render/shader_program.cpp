#include "core/render/shader_program.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

ShaderProgram::~ShaderProgram() {
    destroy();
}

GLuint ShaderProgram::compile(GLenum type, const std::string& path) {
    std::string source = load_source(path);
    if (source.empty())
        return 0;

    // [C1] create shader object and feed GLSL source
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(static_cast<size_t>(std::max(1, logLen)));
        glGetShaderInfoLog(shader, logLen, nullptr, log.data());
        std::cerr << "[Shader] Compile failed (" << path << "): " << log.data() << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

std::string ShaderProgram::load_source(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[Shader] Cannot open " << path << std::endl;
        return {};
    }
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

void ShaderProgram::destroy() {
    if (programId) {
        // [D1] delete previously linked program
        glDeleteProgram(programId);
        programId = 0;
    }
    uniformCache.clear();
}

GLint ShaderProgram::uniform_location(const std::string& name) const {
    if (auto it = uniformCache.find(name); it != uniformCache.end())
        return it->second;
    GLint loc = glGetUniformLocation(programId, name.c_str());
    uniformCache[name] = loc;
    return loc;
}

bool ShaderProgram::load_from_files(const std::string& vertPath, const std::string& fragPath) {
    // [L0] clean up existing program
    destroy();

    // [L1] compile vertex shader
    GLuint vert = compile(GL_VERTEX_SHADER, vertPath);
    if (!vert)
        return false;
    // [L2] compile fragment shader
    GLuint frag = compile(GL_FRAGMENT_SHADER, fragPath);
    if (!frag) {
        glDeleteShader(vert);
        return false;
    }

    // [L3] link shaders into a program object
    programId = glCreateProgram();
    glAttachShader(programId, vert);
    glAttachShader(programId, frag);
    glLinkProgram(programId);

    GLint linked = GL_FALSE;
    glGetProgramiv(programId, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint logLen = 0;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(static_cast<size_t>(std::max(1, logLen)));
        glGetProgramInfoLog(programId, logLen, nullptr, log.data());
        std::cerr << "[Shader] Link failed: " << log.data() << std::endl;
        destroy();
        glDeleteShader(vert);
        glDeleteShader(frag);
        return false;
    }

    // [L4] shaders linked; detach and delete shader objects
    glDetachShader(programId, vert);
    glDetachShader(programId, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);

    uniformCache.clear();
    return true;
}

void ShaderProgram::bind() const {
    glUseProgram(programId);
}

void ShaderProgram::unbind() const {
    glUseProgram(0);
}
