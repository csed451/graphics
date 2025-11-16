#include "core/render/renderer.h"

#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>

#include "core/render/mesh.h"

bool Renderer::init() {
    bool loaded = false, valid = false;
    
    loaded = program.load_from_files("core/render/shaders/basic.vert", "core/render/shaders/basic.frag");
    if (!loaded)
        return false;

    program.bind();
    uModel = program.uniform_location("uModel");
    uView = program.uniform_location("uView");
    uProj = program.uniform_location("uProj");
    uColor = program.uniform_location("uColor");
    uNormal = program.uniform_location("uNormalMatrix");
    uLighting = program.uniform_location("uUseLighting");
    program.unbind();

    valid = uModel >= 0 && uView >= 0 && uProj >= 0 && uColor >= 0 && uNormal >= 0 && uLighting >= 0;
    return valid;
}

void Renderer::shutdown() {
    program.unbind();
}

void Renderer::set_view(const glm::mat4& viewMatrix) {
    view = viewMatrix;
    program.bind();
    glUniformMatrix4fv(uView, 1, GL_FALSE, &view[0][0]);
    program.unbind();
}

void Renderer::set_projection(const glm::mat4& projectionMatrix) {
    projection = projectionMatrix;
    program.bind();
    glUniformMatrix4fv(uProj, 1, GL_FALSE, &projection[0][0]);
    program.unbind();
}

void Renderer::begin_frame() {
    program.bind();
}

void Renderer::end_frame() {
    program.unbind();
}

void Renderer::draw_mesh(const Mesh& mesh,
                         const glm::mat4& modelMatrix,
                         const glm::vec4& color,
                         bool lighting) const {
    if (currentStyle == RenderStyle::HiddenLineWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_DEPTH_TEST);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); 
        glDepthMask(GL_TRUE); // only write to depth buffer
    }

    glUniformMatrix4fv(uModel, 1, GL_FALSE, &modelMatrix[0][0]);
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
    glUniformMatrix3fv(uNormal, 1, GL_FALSE, &normalMatrix[0][0]);
    glUniform4fv(uColor, 1, &color[0]);
    glUniform1i(uLighting, lighting ? 1 : 0);
    mesh.draw();


    if (currentStyle == RenderStyle::HiddenLineWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1.0f, -1.0f);
        glLineWidth(2.0f);
        mesh.draw();
    }
}

void Renderer::draw_raw(GLuint vao,
                        GLsizei vertexCount,
                        GLenum primitive,
                        const glm::mat4& modelMatrix,
                        const glm::vec4& color,
                        bool lighting) const {
    glUniformMatrix4fv(uModel, 1, GL_FALSE, &modelMatrix[0][0]);
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
    glUniformMatrix3fv(uNormal, 1, GL_FALSE, &normalMatrix[0][0]);
    glUniform4fv(uColor, 1, &color[0]);
    glUniform1i(uLighting, lighting ? 1 : 0);
    glBindVertexArray(vao);
    glDrawArrays(primitive, 0, vertexCount);
    glBindVertexArray(0);
}

void Renderer::apply_render_style() {
    switch (currentStyle) {
    case RenderStyle::Opaque:                        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_POLYGON_OFFSET_LINE);
        glLineWidth(1.0f);
        break;
    case RenderStyle::Wireframe:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1.0f, -1.0f);
        glLineWidth(1.2f);
        break;
    case RenderStyle::HiddenLineWireframe:
        break;
    }
}

void Renderer::swich_render_style() {
    currentStyle = static_cast<RenderStyle>((static_cast<int>(currentStyle) + 1) % 3);
    apply_render_style();   
}