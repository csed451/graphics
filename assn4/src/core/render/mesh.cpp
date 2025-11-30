#include "core/render/mesh.h"

#include <GL/glew.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>


// Anonymous namespace for helper functions
namespace {
struct FaceVertex {
    int position = 0;
    int normal = 0;
    int texcoord = 0;
};

void parse_face_token(const std::string& token, FaceVertex& fv) {
    // OBJ indices are 1-based, negative values refer from the end.
    int v = 0, vt = 0, vn = 0;
    size_t firstSlash = token.find('/');
    if (firstSlash == std::string::npos) {
        v = std::stoi(token);
    } else {
        size_t secondSlash = token.find('/', firstSlash + 1);
        v = std::stoi(token.substr(0, firstSlash));
        if (secondSlash == std::string::npos) {
            if (firstSlash + 1 < token.size())
                vt = std::stoi(token.substr(firstSlash + 1));
        } else {
            if (secondSlash > firstSlash + 1)
                vt = std::stoi(token.substr(firstSlash + 1, secondSlash - firstSlash - 1));
            if (secondSlash + 1 < token.size())
                vn = std::stoi(token.substr(secondSlash + 1));
        }
    }

    fv.position = v;
    fv.normal = vn;
    fv.texcoord = vt;
}
}


void Mesh::release_gpu() {
    if (m_vboTangents) {
        glDeleteBuffers(1, &m_vboTangents);
        m_vboTangents = 0;
    }
    if (m_vboTexcoords) {
        glDeleteBuffers(1, &m_vboTexcoords);
        m_vboTexcoords = 0;
    }
    if (m_vboNormals) {
        glDeleteBuffers(1, &m_vboNormals);
        m_vboNormals = 0;
    }
    if (m_vboPositions) {
        glDeleteBuffers(1, &m_vboPositions);
        m_vboPositions = 0;
    }
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}

Mesh::~Mesh() {
    release_gpu();
}

bool Mesh::load_from_obj(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    m_positions.clear();
    m_normals.clear();
    m_texcoords.clear();
    m_tangents.clear();
    release_gpu();

    std::vector<std::array<float, 3>> tempPositions;
    std::vector<std::array<float, 3>> tempNormals;
    std::vector<std::array<float, 2>> tempTexcoords;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            tempPositions.push_back({x, y, z});
        } else if (prefix == "vn") {
            float x, y, z;
            iss >> x >> y >> z;
            tempNormals.push_back({x, y, z});
        } else if (prefix == "vt") {
            float u, v;
            iss >> u >> v;
            tempTexcoords.push_back({u, v});
        } else if (prefix == "f") {
            std::vector<FaceVertex> faceVertices;
            std::string token;
            while (iss >> token) {
                FaceVertex fv{};
                parse_face_token(token, fv);
                faceVertices.push_back(fv);
            }

            if (faceVertices.size() < 3)
                continue;

            auto resolve_index = [&](int idx, const auto& container) -> const auto& {
                if (idx > 0)
                    return container[static_cast<size_t>(idx - 1)];
                // Negative indices refer to relative positions from end
                return container[static_cast<size_t>(container.size() + idx)];
            };

            for (size_t i = 1; i + 1 < faceVertices.size(); ++i) {
                FaceVertex tri[3] = {faceVertices[0], faceVertices[i], faceVertices[i + 1]};
                for (const auto& vert : tri) {
                    const auto& pos = resolve_index(vert.position, tempPositions);
                    m_positions.insert(m_positions.end(), {pos[0], pos[1], pos[2]});
                    if (!tempNormals.empty() && vert.normal != 0) {
                        const auto& nor = resolve_index(vert.normal, tempNormals);
                        m_normals.insert(m_normals.end(), {nor[0], nor[1], nor[2]});
                    }
                    if (!tempTexcoords.empty() && vert.texcoord != 0) {
                        const auto& uv = resolve_index(vert.texcoord, tempTexcoords);
                        m_texcoords.insert(m_texcoords.end(), {uv[0], uv[1]});
                    }
                }
            }
        }
    }

    if (m_positions.empty())
        return false;

    // Generate flat normals if the OBJ did not provide any usable ones
    if (m_normals.size() != m_positions.size()) {
        m_normals.clear();
        m_normals.resize(m_positions.size(), 0.0f);
        for (size_t i = 0; i + 8 < m_positions.size(); i += 9) {
            float ax = m_positions[i];
            float ay = m_positions[i + 1];
            float az = m_positions[i + 2];
            float bx = m_positions[i + 3];
            float by = m_positions[i + 4];
            float bz = m_positions[i + 5];
            float cx = m_positions[i + 6];
            float cy = m_positions[i + 7];
            float cz = m_positions[i + 8];

            float ux = bx - ax;
            float uy = by - ay;
            float uz = bz - az;
            float vx = cx - ax;
            float vy = cy - ay;
            float vz = cz - az;

            float nx = uy * vz - uz * vy;
            float ny = uz * vx - ux * vz;
            float nz = ux * vy - uy * vx;
            float length = std::sqrt(nx * nx + ny * ny + nz * nz);
            if (length <= 0.0f)
                length = 1.0f;
            nx /= length;
            ny /= length;
            nz /= length;

            for (int v = 0; v < 3; ++v) {
                size_t base = i + v * 3;
                m_normals[base] = nx;
                m_normals[base + 1] = ny;
                m_normals[base + 2] = nz;
            }
        }
    }

    // Decide texcoord/tangent availability (drop partial data to keep buffer sizes aligned)
    m_hasTexcoords = (m_texcoords.size() == (m_positions.size() / 3) * 2);
    if (!m_hasTexcoords)
        m_texcoords.clear();

    // Generate tangents if UVs exist
    if (m_hasTexcoords) {
        m_tangents.assign(m_positions.size(), 0.0f);
        for (size_t i = 0; i + 8 < m_positions.size(); i += 9) {
            glm::vec3 p0(m_positions[i],     m_positions[i + 1], m_positions[i + 2]);
            glm::vec3 p1(m_positions[i + 3], m_positions[i + 4], m_positions[i + 5]);
            glm::vec3 p2(m_positions[i + 6], m_positions[i + 7], m_positions[i + 8]);

            size_t uvBase = (i / 3) * 2;
            glm::vec2 uv0(m_texcoords[uvBase],     m_texcoords[uvBase + 1]);
            glm::vec2 uv1(m_texcoords[uvBase + 2], m_texcoords[uvBase + 3]);
            glm::vec2 uv2(m_texcoords[uvBase + 4], m_texcoords[uvBase + 5]);

            glm::vec3 edge1 = p1 - p0;
            glm::vec3 edge2 = p2 - p0;
            glm::vec2 dUV1 = uv1 - uv0;
            glm::vec2 dUV2 = uv2 - uv0;

            float det = dUV1.x * dUV2.y - dUV1.y * dUV2.x;
            float invDet = (std::abs(det) < 1e-6f) ? 1.0f : 1.0f / det;
            glm::vec3 tangent = invDet * (edge1 * dUV2.y - edge2 * dUV1.y);

            for (int v = 0; v < 3; ++v) {
                size_t base = i + v * 3;
                m_tangents[base]     += tangent.x;
                m_tangents[base + 1] += tangent.y;
                m_tangents[base + 2] += tangent.z;
            }
        }

        for (size_t i = 0; i + 2 < m_tangents.size(); i += 3) {
            glm::vec3 t(m_tangents[i], m_tangents[i + 1], m_tangents[i + 2]);
            float len = glm::length(t);
            if (len > 1e-6f)
                t /= len;
            m_tangents[i] = t.x;
            m_tangents[i + 1] = t.y;
            m_tangents[i + 2] = t.z;
        }
    }

    // Setup GPU buffers (Shader-compatible)
    m_vertexCount = static_cast<GLsizei>(m_positions.size() / 3);
    m_hasNormals = (m_normals.size() == m_positions.size());
    m_hasTexcoords = (m_texcoords.size() == static_cast<size_t>(m_vertexCount) * 2);
    m_hasTangents = (m_tangents.size() == m_positions.size());

    // Generate and bind VAO and VBO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vboPositions);

    // Bind VAO and VBO
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);

    // VBO data upload
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_positions.size() * sizeof(float)),
                 m_positions.data(), GL_STATIC_DRAW);
    // VAO attribute setup
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Normals VBO setup if available
    if (m_hasNormals) {
        glGenBuffers(1, &m_vboNormals);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboNormals);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_normals.size() * sizeof(float)),
                     m_normals.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    } else {
        glDisableVertexAttribArray(1);
    }

    // Texcoords VBO setup if available
    if (m_hasTexcoords) {
        glGenBuffers(1, &m_vboTexcoords);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboTexcoords);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_texcoords.size() * sizeof(float)),
                     m_texcoords.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    } else {
        glDisableVertexAttribArray(2);
    }

    // Tangents VBO setup if available
    if (m_hasTangents) {
        glGenBuffers(1, &m_vboTangents);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboTangents);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_tangents.size() * sizeof(float)),
                     m_tangents.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    } else {
        glDisableVertexAttribArray(3);
    }
    
    // Unbind to avoid accidental modification
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

void Mesh::draw() const {
    if (!m_vao)
        return;

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    glBindVertexArray(0);
}


// [free function] Global mesh loading function with caching
std::shared_ptr<Mesh> load_mesh(const std::string& path) {
    static std::unordered_map<std::string, std::weak_ptr<Mesh>> cache;

    if (auto it = cache.find(path); it != cache.end()) {
        if (auto existing = it->second.lock())
            return existing;
    }

    auto mesh = std::make_shared<Mesh>();
    if (!mesh->load_from_obj(path))
        return nullptr;

    cache[path] = mesh;
    return mesh;
}
