#include "background.h"
#include "core/render/renderer.h"
#include <glm/glm.hpp>
#include <vector>

namespace background {

namespace {
GLuint floorVAO = 0;
GLuint floorVBO = 0;
GLuint texOcean = 0;
GLuint wallVAO = 0;
GLuint wallVBO = 0;
GLuint texSky = 0;
GLuint wallFrontVAO = 0;
GLuint wallFrontVBO = 0;
GLuint wallRightVAO = 0;
GLuint wallRightVBO = 0;
GLuint wallBackVAO = 0;
GLuint wallBackVBO = 0;
float cachedMaxCoord = 0.0f;
bool dayModeFlag = true;
float bgScale = 4.0f;
}

void set_day_mode(bool dayMode) {
    dayModeFlag = dayMode;
    // reload texture on next draw
    texOcean = 0;
    texSky = 0;
}

bool is_day_mode() { return dayModeFlag; }
void set_scale(float scale) { bgScale = scale; }
float get_scale() { return bgScale; }

void shutdown() {
    if (floorVBO) glDeleteBuffers(1, &floorVBO);
    if (floorVAO) glDeleteVertexArrays(1, &floorVAO);
    if (wallVBO) glDeleteBuffers(1, &wallVBO);
    if (wallVAO) glDeleteVertexArrays(1, &wallVAO);
    if (wallFrontVBO) glDeleteBuffers(1, &wallFrontVBO);
    if (wallFrontVAO) glDeleteVertexArrays(1, &wallFrontVAO);
    if (wallRightVBO) glDeleteBuffers(1, &wallRightVBO);
    if (wallRightVAO) glDeleteVertexArrays(1, &wallRightVAO);
    if (wallBackVBO) glDeleteBuffers(1, &wallBackVBO);
    if (wallBackVAO) glDeleteVertexArrays(1, &wallBackVAO);
    floorVBO = floorVAO = 0;
    wallVBO = wallVAO = 0;
    wallFrontVBO = wallFrontVAO = 0;
    wallRightVBO = wallRightVAO = 0;
    wallBackVBO = wallBackVAO = 0;
    texOcean = 0;
    texSky = 0;
    cachedMaxCoord = 0.0f;
}

void init(float maxCoord, bool dayMode) {
    if (cachedMaxCoord == maxCoord && floorVAO != 0)
        return;
    shutdown();
    cachedMaxCoord = maxCoord;
    dayModeFlag = dayMode;

    float half = maxCoord * bgScale; // larger than gameplay area
    float z = -maxCoord * 0.5f;   // slightly below to avoid z-fighting

    struct V { glm::vec3 p; glm::vec2 uv; };
    std::vector<V> data = {
        {{-half, -half, z}, {0,0}}, {{ half, -half, z}, {4,0}}, {{ half,  half, z}, {4,4}},
        {{-half, -half, z}, {0,0}}, {{ half,  half, z}, {4,4}}, {{-half,  half, z}, {0,4}},
    };

    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(V), data.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(V), (void*)offsetof(V, uv));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Single side wall: normal +X, center at (-maxCoord*2, 0, 0)
    {
        float yHalf = maxCoord * bgScale;             // width along Y: bgScale*MAX_COORD*2
        float zLow  = -0.5f * maxCoord;
        float zHigh =  0.5f * maxCoord;              // height: MAX_COORD
        float xCenter = -maxCoord * bgScale;
        std::vector<V> data = {
            {{xCenter, -yHalf, zLow }, {0,0}}, {{xCenter,  yHalf, zLow }, {4,0}}, {{xCenter,  yHalf, zHigh}, {4,1}},
            {{xCenter, -yHalf, zLow }, {0,0}}, {{xCenter,  yHalf, zHigh}, {4,1}}, {{xCenter, -yHalf, zHigh}, {0,1}},
        };
        glGenVertexArrays(1, &wallVAO);
        glGenBuffers(1, &wallVBO);
        glBindVertexArray(wallVAO);
        glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
        glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(V), data.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(V), (void*)offsetof(V, uv));
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Right wall: normal -X, center at (+maxCoord*2, 0, 0)
    {
        float yHalf = maxCoord * bgScale;
        float zLow  = -0.5f * maxCoord;
        float zHigh =  0.5f * maxCoord;
        float xCenter =  maxCoord * bgScale;
        std::vector<V> data = {
            {{xCenter,  yHalf, zLow }, {0,0}}, {{xCenter, -yHalf, zLow }, {4,0}}, {{xCenter, -yHalf, zHigh}, {4,1}},
            {{xCenter,  yHalf, zLow }, {0,0}}, {{xCenter, -yHalf, zHigh}, {4,1}}, {{xCenter,  yHalf, zHigh}, {0,1}},
        };
        glGenVertexArrays(1, &wallRightVAO);
        glGenBuffers(1, &wallRightVBO);
        glBindVertexArray(wallRightVAO);
        glBindBuffer(GL_ARRAY_BUFFER, wallRightVBO);
        glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(V), data.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(V), (void*)offsetof(V, uv));
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Front wall: normal -Y, center at (0,  maxCoord*2, 0)
    {
        float xHalf = maxCoord * bgScale;               // width: scaled
        float zLow  = -0.5f * maxCoord;
        float zHigh =  0.5f * maxCoord;              // height: MAX_COORD
        float yCenter = maxCoord * bgScale;
        std::vector<V> data = {
            {{-xHalf, yCenter,  zHigh}, {0,1}}, {{ xHalf, yCenter,  zHigh}, {4,1}}, {{ xHalf, yCenter, zLow}, {4,0}},
            {{-xHalf, yCenter,  zHigh}, {0,1}}, {{ xHalf, yCenter, zLow}, {4,0}}, {{-xHalf, yCenter, zLow}, {0,0}},
        };
        glGenVertexArrays(1, &wallFrontVAO);
        glGenBuffers(1, &wallFrontVBO);
        glBindVertexArray(wallFrontVAO);
        glBindBuffer(GL_ARRAY_BUFFER, wallFrontVBO);
        glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(V), data.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(V), (void*)offsetof(V, uv));
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Back wall: normal +Y, center at (0, -maxCoord*2, 0)
    {
        float half = maxCoord * bgScale;
        float zLow  = -0.5f * maxCoord;
        float zHigh =  0.5f * maxCoord;
        float yCenter = -maxCoord * bgScale;
        std::vector<V> data = {
            {{-half, yCenter, zLow }, {0,0}}, {{ half, yCenter, zLow }, {4,0}}, {{ half, yCenter, zHigh}, {4,1}},
            {{-half, yCenter, zLow }, {0,0}}, {{ half, yCenter, zHigh}, {4,1}}, {{-half, yCenter, zHigh}, {0,1}},
        };
        glGenVertexArrays(1, &wallBackVAO);
        glGenBuffers(1, &wallBackVBO);
        glBindVertexArray(wallBackVAO);
        glBindBuffer(GL_ARRAY_BUFFER, wallBackVBO);
        glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(V), data.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(V), (void*)offsetof(V, uv));
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

static void ensure_textures() {
    if (!texOcean) {
        const char* path = dayModeFlag ? "assets/textures/diffuse_ocean_day.png"
                                       : "assets/textures/diffuse_ocean_night.png";
        texOcean = gRenderer.get_or_load_texture(path);
    }
    if (!texSky) {
        const char* path = dayModeFlag ? "assets/textures/diffuse_sky_day.png"
                                       : "assets/textures/diffuse_sky_night.png";
        texSky = gRenderer.get_or_load_texture(path);
    }
}

void draw() {
    ensure_textures();
    if (floorVAO && texOcean)
        gRenderer.draw_raw(floorVAO, 6, GL_TRIANGLES, glm::mat4(1.0f), glm::vec4(1.0f), false, texOcean);
    GLboolean cullEnabled = glIsEnabled(GL_CULL_FACE);
    if (cullEnabled) glDisable(GL_CULL_FACE); // draw sky quads double-sided
    if (wallVAO && texSky)
        gRenderer.draw_raw(wallVAO, 6, GL_TRIANGLES, glm::mat4(1.0f), glm::vec4(1.0f), false, texSky);
    if (wallRightVAO && texSky)
        gRenderer.draw_raw(wallRightVAO, 6, GL_TRIANGLES, glm::mat4(1.0f), glm::vec4(1.0f), false, texSky);
    if (wallFrontVAO && texSky)
        gRenderer.draw_raw(wallFrontVAO, 6, GL_TRIANGLES, glm::mat4(1.0f), glm::vec4(1.0f), false, texSky);
    if (wallBackVAO && texSky)
        gRenderer.draw_raw(wallBackVAO, 6, GL_TRIANGLES, glm::mat4(1.0f), glm::vec4(1.0f), false, texSky);
    if (cullEnabled) glEnable(GL_CULL_FACE);
}

} // namespace background
