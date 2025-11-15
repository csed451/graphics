#include "core/render/mesh.h"

#include <GL/freeglut.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace {
struct FaceVertex {
    int position = 0;
    int normal = 0;
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
    (void)vt; // texture coordinate unused
}
}

bool Mesh::load_from_obj(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    m_positions.clear();
    m_normals.clear();

    std::vector<std::array<float, 3>> tempPositions;
    std::vector<std::array<float, 3>> tempNormals;

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

            auto resolve_index = [&](int idx, const auto& container) -> const std::array<float,3>& {
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

    return true;
}

void Mesh::draw() const {
    if (m_positions.empty())
        return;

    glPushAttrib(GL_ENABLE_BIT);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, m_positions.data());

    bool hasNormals = m_normals.size() == m_positions.size();
    if (hasNormals) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, m_normals.data());
    }

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_positions.size() / 3));

    if (hasNormals)
        glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glPopAttrib();
}

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
