#pragma once

#include <memory>
#include <string>
#include <vector>

class Mesh {
public:
    Mesh() = default;

    bool load_from_obj(const std::string& path);
    void draw() const;

    const std::vector<float>& positions() const { return m_positions; }
    const std::vector<float>& normals() const { return m_normals; }

private:
    std::vector<float> m_positions; // interleaved as xyz
    std::vector<float> m_normals;   // interleaved as xyz
};

std::shared_ptr<Mesh> load_mesh(const std::string& path);
