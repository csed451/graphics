#pragma once

#include <memory>
#include <string>
#include <vector>

#include <GL/glew.h>

class Mesh {
private:
    std::vector<float> m_positions; // interleaved as xyz
    std::vector<float> m_normals;   // interleaved as xyz
    std::vector<float> m_texcoords; // interleaved as uv
    std::vector<float> m_tangents;  // interleaved as xyz (for normal mapping)
    
    // for Shader-compatible rendering
    GLuint m_vao = 0;
    GLuint m_vboPositions = 0;
    GLuint m_vboNormals = 0;
    GLuint m_vboTexcoords = 0;
    GLuint m_vboTangents = 0;
    GLsizei m_vertexCount = 0;
    bool m_hasNormals = false;
    bool m_hasTexcoords = false;
    bool m_hasTangents = false;

    void release_gpu();

public:
    Mesh() = default;
    ~Mesh();

    // Prevent copy and move semantics
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&) noexcept = delete;
    Mesh& operator=(Mesh&&) noexcept = delete;

    // Getter functions
    const std::vector<float>& positions() const { return m_positions; }
    const std::vector<float>& normals() const { return m_normals; }
    const std::vector<float>& texcoords() const { return m_texcoords; }
    const std::vector<float>& tangents() const { return m_tangents; }
    bool has_normals() const { return m_hasNormals; }
    bool has_texcoords() const { return m_hasTexcoords; }
    bool has_tangents() const { return m_hasTangents; }

    bool load_from_obj(const std::string& path);
    void draw() const;
};

std::shared_ptr<Mesh> load_mesh(const std::string& path);
