#include "core/render/renderer.h"

#include <algorithm>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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

        s.uPointLightCount = s.program.uniform_location("uPointLightCount");
        s.uPointLightPos.fill(-1);
        s.uPointLightColor.fill(-1);
        s.uPointLightIntensity.fill(-1);
        for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
            std::string idx = std::to_string(i);
            s.uPointLightPos[i]       = s.program.uniform_location(("uPointLights[" + idx + "].position").c_str());
            s.uPointLightColor[i]     = s.program.uniform_location(("uPointLights[" + idx + "].color").c_str());
            s.uPointLightIntensity[i] = s.program.uniform_location(("uPointLights[" + idx + "].intensity").c_str());
        }

        // related to Texture uniforms
        s.uViewPos              = s.program.uniform_location("uViewPos");
        s.uUseTexture           = s.program.uniform_location("uUseTexture");
        s.uDiffuseMap           = s.program.uniform_location("uDiffuseMap");
        s.uUseNormalMap         = s.program.uniform_location("uUseNormalMap");
        s.uNormalMap            = s.program.uniform_location("uNormalMap");

        // related to shadow mapping (depth only)
        s.uLightSpaceMatrix     = s.program.uniform_location("uLightSpaceMatrix");
        s.uShadowMap            = s.program.uniform_location("uShadowMap");

        if (s.uDiffuseMap >= 0) glUniform1i(s.uDiffuseMap, 0);
        if (s.uNormalMap >= 0) glUniform1i(s.uNormalMap, 1);
        s.program.unbind();

        return s.uModel >= 0 && s.uView >= 0 && s.uProj >= 0 && s.uColor >= 0 && s.uNormal >= 0 && s.uLighting >= 0 
        || s.uModel >= 0 && s.uLightSpaceMatrix >= 0;
    };

    bool is_valid = true;
    is_valid &= load_set(ShadingMode::Gouraud, "core/render/shaders/gouraud.vert", "core/render/shaders/gouraud.frag");
    is_valid &= load_set(ShadingMode::Phong, "core/render/shaders/phong.vert", "core/render/shaders/phong.frag");
    is_valid &= load_set(ShadingMode::PhongNormalMap, "core/render/shaders/phong_nm.vert", "core/render/shaders/phong_nm.frag");
    is_valid &= load_set(ShadingMode::DepthOnly, "core/render/shaders/depth.vert", "core/render/shaders/depth.frag");


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

void Renderer::set_lights(const DirectionalLight& dir, const std::vector<PointLight>& points) {
    dirLight = dir;
    pointLightCount = std::min(static_cast<int>(points.size()), MAX_POINT_LIGHTS);
    for (int i = 0; i < pointLightCount; ++i)
        pointLights[i] = points[i];

    for (auto& s : shaders) {
        s.program.bind();
        if (s.uDirLightDir >= 0)         glUniform3fv(s.uDirLightDir, 1, &dirLight.direction[0]);
        if (s.uDirLightColor >= 0)       glUniform3fv(s.uDirLightColor, 1, &dirLight.color[0]);
        if (s.uDirLightIntensity >= 0)   glUniform1f(s.uDirLightIntensity, dirLight.intensity);

        if (s.uPointLightCount >= 0)
            glUniform1i(s.uPointLightCount, pointLightCount);

        for (int i = 0; i < pointLightCount; ++i) {
            if (s.uPointLightPos[i] >= 0)       glUniform3fv(s.uPointLightPos[i], 1, &pointLights[i].position[0]);
            if (s.uPointLightColor[i] >= 0)     glUniform3fv(s.uPointLightColor[i], 1, &pointLights[i].color[0]);
            if (s.uPointLightIntensity[i] >= 0) glUniform1f(s.uPointLightIntensity[i], pointLights[i].intensity);
        }
    }
    shaders[static_cast<int>(currentShading)].program.bind(); // keep active shader bound
}

void Renderer::set_light_space_matrix() {
    // ...
    // 광원 시야 영역 정의 (장면에 맞게 크기 조절 필요)
    // 월드 크기 100x100에 맞게 범위 확장: [-60, 60] (여유 공간 확보)
    float near_plane = 1.0f, far_plane = 100.0f; // 깊이 범위도 충분히 확장 (예: 100.0f)
    glm::mat4 lightProjection = glm::ortho(-60.0f, 60.0f, -60.0f, 60.0f, near_plane, far_plane);

    // 2. 뷰 행렬 (View Matrix) - 광원 시점
    // 타겟 위치를 여전히 씬 중앙(0, 0, 0)으로 설정한다고 가정합니다.
    // 카메라 위치도 씬 크기에 맞게 멀리 설정하는 것이 좋습니다.
    // 현재 dirLight.direction = glm::normalize(glm::vec3(0.0f, 0.0f, -0.1f)); 이므로,
    // 빛이 Z축을 따라옵니다. 이 경우 * 2.0f 대신 * 50.0f 등으로 더 멀리 설정해야
    // lightView가 씬 전체를 포괄할 가능성이 높아집니다.
    glm::mat4 lightView = glm::lookAt(-dirLight.direction * 10.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    
    // lightProjection과 lightView 행렬을 결합하여 광원 공간 행렬 생성
    glm::mat4 mat = lightProjection * lightView;

    for (auto& s : shaders) {
        s.program.bind();
        if (s.uLightSpaceMatrix >= 0)
            glUniformMatrix4fv(s.uLightSpaceMatrix, 1, GL_FALSE, glm::value_ptr(mat));
    }
    shaders[static_cast<int>(currentShading)].program.bind(); // keep active shader bound
}

void Renderer::set_shadow_map(GLuint depthMapTexture) {
    // 1. 섀도우 맵을 사용할 텍스처 유닛 활성화 (GL_TEXTURE2, 즉 인덱스 2 사용)
    glActiveTexture(GL_TEXTURE2); 
    
    // 2. 섀도우 맵 텍스처 바인딩
    // depthMapTexture는 함수 외부에서 생성된 FBO의 깊이 텍스처 ID입니다.
    glBindTexture(GL_TEXTURE_2D, depthMapTexture); 
    
    // 3. 셰이더 유니폼 변수 uShadowMap에 텍스처 유닛 인덱스 (2) 전달
    for (auto& s : shaders) {
        // 셰이더가 활성화되어 있는 동안 해당 셰이더의 유니폼에 값을 설정합니다.
        // 이 함수가 호출되는 시점에 gRenderer.apply_render_style()을 통해
        // 최종 셰이더(phong.frag)가 바인딩되어 있어야 합니다.
        s.program.bind();
        if (s.uShadowMap >= 0) {
            // ⭐ 수정: 텍스처 유닛 인덱스 2 전달
            glUniform1i(s.uShadowMap, 2); 
        }
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
    
    if (currentShading == ShadingMode::DepthOnly) {
        shader->program.bind();
        if (shader->uModel >= 0) glUniformMatrix4fv(shader->uModel, 1, GL_FALSE, &modelMatrix[0][0]);

        mesh.draw();
        return;
    }


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
                        bool lighting,
                        GLuint diffuseTex,
                        GLuint normalTex,
                        bool useNormalMap) const {
    const auto& shader = shaders[static_cast<int>(currentShading)];


    shader.program.bind();
    if (shader.uModel >= 0) glUniformMatrix4fv(shader.uModel, 1, GL_FALSE, &modelMatrix[0][0]);
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
    if (shader.uNormal >= 0) glUniformMatrix3fv(shader.uNormal, 1, GL_FALSE, &normalMatrix[0][0]);
    if (shader.uColor >= 0) glUniform4fv(shader.uColor, 1, &color[0]);
    if (shader.uLighting >= 0) glUniform1i(shader.uLighting, lighting ? 1 : 0);
    if (shader.uUseTexture >= 0) glUniform1i(shader.uUseTexture, diffuseTex ? 1 : 0);
    if (shader.uDiffuseMap >= 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTex ? diffuseTex : whiteTexture);
        glUniform1i(shader.uDiffuseMap, 0);
    }
    bool enableNM = useNormalMap && normalTex;
    if (shader.uUseNormalMap >= 0) glUniform1i(shader.uUseNormalMap, enableNM ? 1 : 0);
    if (shader.uNormalMap >= 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, enableNM ? normalTex : whiteTexture);
        glUniform1i(shader.uNormalMap, 1);
    }

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
    case ShadingMode::DepthOnly: return "Depth Only";
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
