#include "core/render/renderer.h"

#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>

#include "core/render/mesh.h"

namespace {
    struct GLRenderState {
        GLint polygonMode[2] = {GL_FILL, GL_FILL};
        GLboolean colorMask[4] = {GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE};
        GLboolean depthMask = GL_TRUE;
        GLboolean depthTest = GL_TRUE;
        GLboolean polygonOffsetLine = GL_FALSE;
        GLfloat lineWidth = 1.0f;
        GLfloat polygonOffsetFactor = 0.0f;
        GLfloat polygonOffsetUnits = 0.0f;
    };

    GLRenderState capture_state() {
        GLRenderState state;
        glGetIntegerv(GL_POLYGON_MODE, state.polygonMode);
        glGetBooleanv(GL_COLOR_WRITEMASK, state.colorMask);
        glGetBooleanv(GL_DEPTH_WRITEMASK, &state.depthMask);
        state.depthTest = glIsEnabled(GL_DEPTH_TEST);
        state.polygonOffsetLine = glIsEnabled(GL_POLYGON_OFFSET_LINE);
        glGetFloatv(GL_LINE_WIDTH, &state.lineWidth);
        glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &state.polygonOffsetFactor);
        glGetFloatv(GL_POLYGON_OFFSET_UNITS, &state.polygonOffsetUnits);
        return state;
    }

    void restore_state(const GLRenderState& state) {
        glPolygonMode(GL_FRONT, state.polygonMode[0]);
        glPolygonMode(GL_BACK, state.polygonMode[1]);
        glColorMask(state.colorMask[0], state.colorMask[1], state.colorMask[2], state.colorMask[3]);
        glDepthMask(state.depthMask);
        if (state.depthTest)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
        if (state.polygonOffsetLine)
            glEnable(GL_POLYGON_OFFSET_LINE);
        else
            glDisable(GL_POLYGON_OFFSET_LINE);
        glLineWidth(state.lineWidth);
        glPolygonOffset(state.polygonOffsetFactor, state.polygonOffsetUnits);
    }

    bool is_triangle_primitive(GLenum primitive) {
        return primitive == GL_TRIANGLES || primitive == GL_TRIANGLE_STRIP || primitive == GL_TRIANGLE_FAN;
    }
}

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
    glUniformMatrix4fv(uModel, 1, GL_FALSE, &modelMatrix[0][0]);
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
    glUniformMatrix3fv(uNormal, 1, GL_FALSE, &normalMatrix[0][0]);
    glUniform4fv(uColor, 1, &color[0]);
    glUniform1i(uLighting, lighting ? 1 : 0);

    if (currentStyle != RenderStyle::HiddenLineWireframe) {
        mesh.draw();
        return;
    }

    GLRenderState saved = capture_state();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_TRUE);
    mesh.draw(); // depth pre-pass

    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_FALSE);
    glPolygonOffset(-1.0f, -1.0f);
    glLineWidth(2.0f);
    mesh.draw(); // outline pass

    restore_state(saved);
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

    bool hiddenLine = currentStyle == RenderStyle::HiddenLineWireframe && is_triangle_primitive(primitive);
    if (!hiddenLine) {
        glDrawArrays(primitive, 0, vertexCount);
        glBindVertexArray(0);
        return;
    }

    GLRenderState saved = capture_state();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_TRUE);
    glDrawArrays(primitive, 0, vertexCount);

    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_FALSE);
    glPolygonOffset(-1.0f, -1.0f);
    glLineWidth(2.0f);
    glDrawArrays(primitive, 0, vertexCount);

    restore_state(saved);
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
