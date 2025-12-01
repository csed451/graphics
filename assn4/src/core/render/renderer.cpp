#include "core/render/renderer.h"

#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>

#include "core/render/mesh.h"
#include "core/render/texture.h"

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
    // Load and cache shader programs per shading mode
    auto load_set = [&](ShadingMode mode, const char* vert, const char* frag) -> bool {
        auto& s = shaders[static_cast<int>(mode)];
        bool loaded = s.program.load_from_files(vert, frag);
        if (!loaded)
            return false;

        s.program.bind();
        s.uModel                = s.program.uniform_location("uModel");
        s.uView                 = s.program.uniform_location("uView");
        s.uProj                 = s.program.uniform_location("uProj");
        s.uColor                = s.program.uniform_location("uColor");
        s.uNormal               = s.program.uniform_location("uNormalMatrix");

        // related to Lighting uniforms
        s.uLighting             = s.program.uniform_location("uUseLighting");
        s.uDirLightDir          = s.program.uniform_location("uDirLight.direction");
        s.uDirLightColor        = s.program.uniform_location("uDirLight.color");
        s.uDirLightIntensity    = s.program.uniform_location("uDirLight.intensity");
        s.uPointLightPos        = s.program.uniform_location("uPointLight.position");
        s.uPointLightColor      = s.program.uniform_location("uPointLight.color");
        s.uPointLightIntensity  = s.program.uniform_location("uPointLight.intensity");

        // related to Texture uniforms
        s.uViewPos              = s.program.uniform_location("uViewPos");
        s.uUseTexture           = s.program.uniform_location("uUseTexture");
        s.uDiffuseMap           = s.program.uniform_location("uDiffuseMap");
        s.uUseNormalMap         = s.program.uniform_location("uUseNormalMap");
        s.uNormalMap            = s.program.uniform_location("uNormalMap");

        if (s.uDiffuseMap >= 0) glUniform1i(s.uDiffuseMap, 0);
        if (s.uNormalMap >= 0) glUniform1i(s.uNormalMap, 1);
        s.program.unbind();

        return s.uModel >= 0 && s.uView >= 0 && s.uProj >= 0 && s.uColor >= 0 && s.uNormal >= 0 && s.uLighting >= 0;
    };

    bool is_valid = true;
    is_valid &= load_set(ShadingMode::Gouraud, "core/render/shaders/gouraud.vert", "core/render/shaders/gouraud.frag");
    is_valid &= load_set(ShadingMode::Phong, "core/render/shaders/phong.vert", "core/render/shaders/phong.frag");
    is_valid &= load_set(ShadingMode::PhongNormalMap, "core/render/shaders/phong_nm.vert", "core/render/shaders/phong_nm.frag");

    if (!is_valid)
        return false;

    // Tiny 1x1 white fallback texture for untextured draws
    glGenTextures(1, &whiteTexture);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    const unsigned char whitePixel[4] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    currentShading = ShadingMode::Gouraud; // start with Gouraud -> W cycles Phong -> NormalMap
    apply_render_style();
    return true;
}

void Renderer::shutdown() {
    for (auto& s : shaders)
        s.program.unbind();
    for (auto& kv : textureCache) {
        if (kv.second.id)
            glDeleteTextures(1, &kv.second.id);
    }
    textureCache.clear();
    if (whiteTexture) {
        glDeleteTextures(1, &whiteTexture);
        whiteTexture = 0;
    }
}

void Renderer::set_view(const glm::mat4& viewMatrix) {
    view = viewMatrix;
    for (auto& s : shaders) {
        s.program.bind();
        if (s.uView >= 0)
            glUniformMatrix4fv(s.uView, 1, GL_FALSE, &view[0][0]);
    }
    shaders[static_cast<int>(currentShading)].program.bind(); // keep active shader bound
}

void Renderer::set_projection(const glm::mat4& projectionMatrix) {
    projection = projectionMatrix;
    for (auto& s : shaders) {
        s.program.bind();
        if (s.uProj >= 0)
            glUniformMatrix4fv(s.uProj, 1, GL_FALSE, &projection[0][0]);
    }
    shaders[static_cast<int>(currentShading)].program.bind(); // keep active shader bound
}

void Renderer::set_view_position(const glm::vec3& pos) {
    viewPos = pos;
    for (auto& s : shaders) {
        s.program.bind();
        if (s.uViewPos >= 0)
            glUniform3fv(s.uViewPos, 1, &viewPos[0]);
    }
    shaders[static_cast<int>(currentShading)].program.bind(); // keep active shader bound
}

void Renderer::set_lights(const DirectionalLight& dir, const PointLight& point) {
    dirLight = dir;
    pointLight = point;
    for (auto& s : shaders) {
        s.program.bind();
        if (s.uDirLightDir >= 0)         glUniform3fv(s.uDirLightDir, 1, &dirLight.direction[0]);
        if (s.uDirLightColor >= 0)       glUniform3fv(s.uDirLightColor, 1, &dirLight.color[0]);
        if (s.uDirLightIntensity >= 0)   glUniform1f(s.uDirLightIntensity, dirLight.intensity);
        if (s.uPointLightPos >= 0)       glUniform3fv(s.uPointLightPos, 1, &pointLight.position[0]);
        if (s.uPointLightColor >= 0)     glUniform3fv(s.uPointLightColor, 1, &pointLight.color[0]);
        if (s.uPointLightIntensity >= 0) glUniform1f(s.uPointLightIntensity, pointLight.intensity);
    }
    shaders[static_cast<int>(currentShading)].program.bind(); // keep active shader bound
}

void Renderer::begin_frame() {
    shaders[static_cast<int>(currentShading)].program.bind();
}

void Renderer::end_frame() {
    shaders[static_cast<int>(currentShading)].program.unbind();
}

void Renderer::draw_mesh(const Mesh& mesh,
                         const glm::mat4& modelMatrix,
                         const glm::vec4& color,
                         bool lighting,
                         GLuint diffuseTex,
                         GLuint normalTex,
                         bool useNormalMap) const {
    const ShaderHandles* shader = &shaders[static_cast<int>(currentShading)];
    bool needsFallback = currentShading == ShadingMode::PhongNormalMap &&
                         (!mesh.has_texcoords() || !mesh.has_tangents());
    if (needsFallback)
        shader = &shaders[static_cast<int>(ShadingMode::Phong)];

    shader->program.bind();

    if (shader->uModel >= 0) glUniformMatrix4fv(shader->uModel, 1, GL_FALSE, &modelMatrix[0][0]);
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
    if (shader->uNormal >= 0) glUniformMatrix3fv(shader->uNormal, 1, GL_FALSE, &normalMatrix[0][0]);
    if (shader->uColor >= 0) glUniform4fv(shader->uColor, 1, &color[0]);
    if (shader->uLighting >= 0) glUniform1i(shader->uLighting, lighting ? 1 : 0);

    if (shader->uUseTexture >= 0) glUniform1i(shader->uUseTexture, diffuseTex ? 1 : 0);
    if (shader->uDiffuseMap >= 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTex ? diffuseTex : whiteTexture);
        glUniform1i(shader->uDiffuseMap, 0);
    }

    bool enableNormal = useNormalMap && !needsFallback && normalTex != 0;
    if (shader->uUseNormalMap >= 0) glUniform1i(shader->uUseNormalMap, enableNormal ? 1 : 0);
    if (shader->uNormalMap >= 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, enableNormal ? normalTex : whiteTexture);
        glUniform1i(shader->uNormalMap, 1);
    }

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
    const auto& shader = shaders[static_cast<int>(currentShading)];
    shader.program.bind();
    if (shader.uModel >= 0) glUniformMatrix4fv(shader.uModel, 1, GL_FALSE, &modelMatrix[0][0]);
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
    if (shader.uNormal >= 0) glUniformMatrix3fv(shader.uNormal, 1, GL_FALSE, &normalMatrix[0][0]);
    if (shader.uColor >= 0) glUniform4fv(shader.uColor, 1, &color[0]);
    if (shader.uLighting >= 0) glUniform1i(shader.uLighting, lighting ? 1 : 0);
    // force untextured for raw draws to avoid stale state from textured meshes
    if (shader.uUseTexture >= 0) glUniform1i(shader.uUseTexture, 0);
    if (shader.uUseNormalMap >= 0) glUniform1i(shader.uUseNormalMap, 0);

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

void Renderer::switch_render_style() {
    currentStyle = static_cast<RenderStyle>((static_cast<int>(currentStyle) + 1) % 3);
    apply_render_style();   
}

void Renderer::set_shading_mode(ShadingMode mode) {
    currentShading = mode;
    auto& s = shaders[static_cast<int>(currentShading)];
    s.program.bind();
    if (s.uDiffuseMap >= 0) glUniform1i(s.uDiffuseMap, 0);
    if (s.uNormalMap >= 0) glUniform1i(s.uNormalMap, 1);
}

void Renderer::switch_shading_mode() {
    int next = (static_cast<int>(currentShading) + 1) % 3;
    set_shading_mode(static_cast<ShadingMode>(next));
}

const char* Renderer::shading_mode_label() const {
    switch (currentShading) {
    case ShadingMode::Gouraud: return "Gouraud";
    case ShadingMode::Phong: return "Phong";
    case ShadingMode::PhongNormalMap: return "Phong + Normal Map";
    default: return "Unknown";
    }
}

GLuint Renderer::get_or_load_texture(const std::string& path) {
    if (auto it = textureCache.find(path); it != textureCache.end())
        return it->second.id;

    Texture2D tex = load_texture_png(path);
    if (tex.id != 0) {
        textureCache[path] = tex;
        return tex.id;
    }
    return 0;
}
