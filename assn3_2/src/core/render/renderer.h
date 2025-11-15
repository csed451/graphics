#pragma once

#include <glm/glm.hpp>

#include "core/render/shader_program.h"

class Mesh;

class Renderer {
private:
    ShaderProgram program;
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    GLint uModel = -1;
    GLint uView = -1;
    GLint uProj = -1;
    GLint uColor = -1;
    GLint uNormal = -1;
    GLint uLighting = -1;

public:
    bool init();
    void shutdown();

    void set_view(const glm::mat4& viewMatrix);
    void set_projection(const glm::mat4& projectionMatrix);

    void begin_frame();
    void end_frame();

    void draw_mesh(const Mesh& mesh,
                   const glm::mat4& modelMatrix,
                   const glm::vec4& color,
                   bool lighting = true) const;

    void draw_raw(GLuint vao,
                  GLsizei vertexCount,
                  GLenum primitive,
                  const glm::mat4& modelMatrix,
                  const glm::vec4& color,
                  bool lighting = false) const;
};

inline Renderer gRenderer;
