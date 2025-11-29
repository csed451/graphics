#pragma once

#include <glm/glm.hpp>

#include "core/render/shader_program.h"

class Mesh;

enum class RenderStyle { Opaque, Wireframe, HiddenLineWireframe };

struct DirectionalLight {
    glm::vec3 direction = glm::vec3(-0.4f, 0.7f, 0.5f);
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
};

struct PointLight {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
};

class Renderer {
private:
    ShaderProgram program;
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::vec3 viewPos = glm::vec3(0.0f);
    DirectionalLight dirLight;
    PointLight pointLight;

    GLint uModel = -1;
    GLint uView = -1;
    GLint uProj = -1;
    GLint uColor = -1;
    GLint uNormal = -1;
    GLint uLighting = -1;
    GLint uDirLightDir = -1;
    GLint uDirLightColor = -1;
    GLint uDirLightIntensity = -1;
    GLint uPointLightPos = -1;
    GLint uPointLightColor = -1;
    GLint uPointLightIntensity = -1;
    GLint uViewPos = -1;

    RenderStyle currentStyle = RenderStyle::Opaque;

public:
    bool init();
    void shutdown();

    void set_view(const glm::mat4& viewMatrix);
    void set_projection(const glm::mat4& projectionMatrix);
    void set_view_position(const glm::vec3& pos);
    void set_lights(const DirectionalLight& dir, const PointLight& point);

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


    void apply_render_style();
    void switch_render_style();
};

inline Renderer gRenderer;
